/*
** Copyright 2018 The Earlham Institute
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/
#include <stdio.h>
#include <string.h>

#include "rodsClient.h"

#include "byte_buffer.h"

/*
 * STATIC VARIABLES
 */

static bool s_verbose_flag = false;


/*
 * STATIC DECLARATIONS
 */

static char *GetParentPath (const char *path_s);

static const char *GetLocalName (const char *path_s);

static int GetEntries (char *path_s, const char *partial_match_s, rcComm_t *connection_p, ByteBuffer *buffer_p);


/*
 * DEFINITIONS
 */

int main (int argc, char *argv [])
{
	int res = 0;
	rodsEnv env;
	int status = getRodsEnv (&env);

	/*
	 * Parse the command line arguments.
	 */
	if (argc > 1)
		{
			int i;

			for (i = 1; i < argc - 1; ++ i)
				{
					if (*argv [i] == '-')
						{
							char c = * (argv [i] + 1);

							if (c == 'v')
								{
									s_verbose_flag = true;
								}

						}		/* if (*argv [i] == '-') */

				}		/* for (i = 1; i < argc - 1; ++ i) */

		}		/* if (argc > 1) */

	if (status >= 0)
		{
			rErrMsg_t err;
			rcComm_t *connection_p = rcConnect (env.rodsHost, env.rodsPort, env.rodsUserName, env.rodsZone,	0, &err);

			if (connection_p)
				{
					status = clientLogin (connection_p, NULL, NULL);

					if (status == 0)
						{
							ByteBuffer *buffer_p = AllocateByteBuffer (1024);

							if (buffer_p)
								{
									char *parent_s = NULL;
									char *input_path_s = (char *) argv [argc - 1];

									/*
									 * Get the matches assuming the path is a complete valid path
									 */
									int num_entries = GetEntries (input_path_s, NULL, connection_p, buffer_p);

									if (num_entries >= 0)
										{
											if (s_verbose_flag)
												{
													printf ("added %d entries for matches of \"%s\"\n", num_entries, input_path_s);
												}
										}
									else
										{

										}

									/*
									 * Now try it as an incomplete path
									 */
									parent_s = GetParentPath (input_path_s);

									if (parent_s)
										{
											const char *local_s = GetLocalName (input_path_s);

											if (local_s)
												{
													int num_partial_entries = GetEntries (parent_s, local_s, connection_p, buffer_p);

													if (num_partial_entries >= 0)
														{
															if (s_verbose_flag)
																{
																	printf ("added %d entries for partial matches of \"%s\"\n", num_partial_entries, input_path_s);
																	num_entries += num_partial_entries;
																}

														}		/* if (num_partial_entries >= 0) */

												}		/* if (local_s) */

											free (parent_s);
										}		/* if (parent_s) */

									if (GetByteBufferSize (buffer_p) > 0)
										{
											const char *data_s = GetByteBufferData (buffer_p);

											if (s_verbose_flag)
												{
													printf ("final value: \"%s\"\n", data_s);
												}

											/*
											 * Print out the space-separated matching paths
											 */
											printf ("%s", data_s);
										}

									FreeByteBuffer (buffer_p);
								}		/* if (buffer_p) */
							else
								{
									if (s_verbose_flag)
										{
											fprintf (stderr, "Failed to allocate buffer to store entries");
										}
								}

						}		/* if (status == 0) */
					else
						{
							if (s_verbose_flag)
								{
									fprintf (stderr, "Failed to log in to iRODS \"%s\"\n", rodsErrorName (status, NULL));
								}
						}

					rcDisconnect (connection_p);
				}		/* if (connection_p) */
			else
				{
					if (s_verbose_flag)
						{
							fprintf (stderr, "Failed to connect to iRODS \"%s\"\n", rodsErrorName (status, NULL));
						}
				}

		}		/* if (status >= 0) */
	else
		{
			if (s_verbose_flag)
				{
					fprintf (stderr, "Failed to get iRODS Environment variables \"%s\"\n", rodsErrorName (status, NULL));
				}

			res = 10;
		}


	return res;
}


static int GetEntries (char *path_s, const char *partial_match_s, rcComm_t *connection_p, ByteBuffer *buffer_p)
{
	int num_entries = -1;
	collHandle_t coll_handle;
	int flags = DATA_QUERY_FIRST_FG;
	const size_t partial_match_length = (partial_match_s != NULL) ? strlen (partial_match_s) : 0;
	int status;

	memset (&coll_handle, 0, sizeof (collHandle_t));

	status = rclOpenCollection (connection_p, path_s, flags, &coll_handle);

	if (s_verbose_flag)
		{
			printf ("Opening \"%s\" %s\n", path_s, (status >= 0) ? "succeeded" : "failed");
		}


	if (status >= 0)
		{
			collEnt_t coll_entry;
			size_t index = 0;
			bool success_flag = true;

			do
				{
					status = rclReadCollection (connection_p, &coll_handle, &coll_entry);

					if (status >= 0)
						{
							bool add_flag = true;

							if (partial_match_s)
								{
									const char *local_s = GetLocalName (coll_entry.collName);

									if (local_s)
										{
											if (strncmp (local_s, partial_match_s, partial_match_length) != 0)
												{
													add_flag = false;
												}
										}
								}

							if (add_flag)
								{
									const char *data_s = GetByteBufferData (buffer_p);

									if (index > 0)
										{
											if (!AppendStringToByteBuffer (buffer_p, " "))
												{
													if (s_verbose_flag)
														{
															fprintf (stderr, "AppendStringToByteBuffer failed");
														}

													success_flag = false;
												}
										}

									if (success_flag)
										{
											if (AppendStringsToByteBuffer (buffer_p, coll_entry.collName, "/", NULL))
												{
													if (coll_entry.objType == DATA_OBJ_T)
														{
															if (!AppendStringsToByteBuffer (buffer_p, coll_entry.dataName, NULL))
																{
																	if (s_verbose_flag)
																		{
																			fprintf (stderr, "AppendStringToByteBuffer failed");
																		}

																	success_flag = false;
																}		/* if (!AppendStringsToByteBuffer (buffer_p, coll_entry.dataName, NULL)) */

														}		/* if (coll_entry.objType == DATA_OBJ_T) */

													if (s_verbose_flag)
														{
															printf ("[%lu]: \"%s\" %d\n", index, data_s, success_flag);
														}

												}		/* if (AppendStringsToByteBuffer (buffer_p, coll_entry.collName, "/", NULL)) */
											else
												{
													if (s_verbose_flag)
														{
															fprintf (stderr, "AppendStringToByteBuffer failed");
														}

													success_flag = false;
												}

										}		/* if (success_flag) */

									++ index;
								}		/* if (add_flag) */

						}		/* if (status >= 0) */
					else
						{
							if (status != CAT_NO_ROWS_FOUND)
								{
									/* We have an error */
									if (s_verbose_flag)
										{
											const char *error_status_s = rodsErrorName (status, NULL);
											fprintf (stderr, "rclReadCollection error, %s", error_status_s);
										}
								}
						}

				}
			while ((status >= 0) && (success_flag == true));

			if (success_flag)
				{
					num_entries = index;
				}

			rclCloseCollection (&coll_handle);
		}		/* if (status >= 0) */


	return num_entries;
}


/**
 * Get the local name of a data object or collection.
 *
 * This is the value after the last "/" in the given path.
 *
 * @param path_s The path to use.
 * @return The local name or <code>NULL</code> upon error.
 */
static const char *GetLocalName (const char *path_s)
{
	const char *res_s = path_s;
	size_t len = strlen (path_s);

	if (len != 0)
		{
			size_t i = len - 1;
			const char *ptr = path_s + i;

			while (i > 0)
				{
					if (*ptr == '/')
						{
							res_s = ptr + 1;
							i = 0;
						}
					else
						{
							-- i;
							-- ptr;
						}
				}
		}

	return res_s;
}


/**
 * Get the parent of a data object or collection.
 *
 * This is the value upto last "/" in the given path.
 *
 * @param path_s The path to use.
 * @return The parent or <code>NULL</code> upon error.
 */
static char *GetParentPath (const char *path_s)
{
	char *res_s = NULL;
	size_t len = strlen (path_s);

	if (len > 1)
		{
			size_t i = len - 1;
			const char *ptr = path_s + i;

			while (ptr >= path_s)
				{
					if (*ptr == '/')
						{
							/*
							 * If we are at the root, make sure we include the / separator.
							 * For all other levels, leave it off
							 */
							if (i == 0)
								{
									++ i;
								}

							res_s = (char *) malloc ((i + 1) * sizeof (char));

							if (res_s)
								{
									strncpy (res_s, path_s, i);
									* (res_s + i) = '\0';
								}

							ptr = path_s - 1;
						}
					else
						{
							-- i;
							-- ptr;
						}

				}

		}

	return res_s;
}
