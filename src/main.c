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


/*
 * DEFINITIONS
 */

int main (int argc, char *argv [])
{
	int res = 0;
	rodsEnv env;
	int status = getRodsEnv (&env);
	const char *error_info_s = NULL;
	const char *error_status_s = NULL;

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
						}
				}
		}

	if (status >= 0)
		{
			rErrMsg_t err;
			rcComm_t *connection_p = rcConnect (env.rodsHost, env.rodsPort, env.rodsUserName, env.rodsZone,	0, &err);

			if (connection_p)
				{
					status = clientLogin (connection_p, NULL, NULL);

					if (status == 0)
						{
							collHandle_t coll_handle;
							char *input_path_s = (char *) argv [argc - 1];
							int flags = DATA_QUERY_FIRST_FG;
							const char *partial_match_s = NULL;
							size_t partial_match_length = 0;

							memset (&coll_handle, 0, sizeof (collHandle_t));

							if (s_verbose_flag)
								{
									printf ("Opening \"%s\"\n", input_path_s);
								}

							/*
							 * Start by trying the input path as a complete
							 * and valid path
							 */
							status = rclOpenCollection (connection_p, input_path_s, flags, &coll_handle);

							if (status < 0)
								{
									/*
									 * Now try it as an incomplete path
									 */
									char *parent_s = GetParentPath (input_path_s);

									if (s_verbose_flag)
										{
											printf ("Opening \"%s\"\n", parent_s);
										}

									if (parent_s)
										{
											status = rclOpenCollection (connection_p, parent_s, flags, &coll_handle);

											if (status < 0)
												{
													error_info_s = "rclOpenCollection";
													error_status_s = rodsErrorName (status, NULL);
												}		/* if (handle < 0) */
											else
												{
													partial_match_s = GetLocalName (input_path_s);

													if (partial_match_s)
														{
															partial_match_length = strlen (partial_match_s);
														}
												}

											free (parent_s);
										}		/* if (parent_s) */

								}

							if (status >= 0)
								{
									ByteBuffer *buffer_p = AllocateByteBuffer (1024);

									if (buffer_p)
										{
											collEnt_t coll_entry;
											size_t index = 0;
											bool success_flag = true;

											do
												{
													status = rclReadCollection (connection_p, &coll_handle, &coll_entry);

													if (status < 0)
														{
															if (status != CAT_NO_ROWS_FOUND)
																{
																	/* Failed to read collection */
																	error_info_s = "rclReadCollection";
																	error_status_s = rodsErrorName (status, NULL);

																	res = 10;
																}
														}
													else
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
																					success_flag = false;
																				}
																		}

																	if (success_flag)
																		{
																			if (AppendStringsToByteBuffer (buffer_p, coll_entry.collName, "/", NULL))
																				{
																					if (coll_entry.objType == DATA_OBJ_T)
																						{
																							success_flag = AppendStringsToByteBuffer (buffer_p, coll_entry.dataName, NULL);
																						}

																					if (s_verbose_flag)
																						{
																							printf ("[%lu]: \"%s\" %d\n", index, data_s, success_flag);
																						}

																				}


																		}		/* if (success_flag) */

																	++ index;
																}		/* if (add_flag) */
														}

												}
											while (status >= 0);

											if (GetByteBufferSize (buffer_p))
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

									rclCloseCollection (&coll_handle);
								}		/* if (handle >= 0) */
							else
								{
									error_info_s = "rclOpenCollection";
									error_status_s = rodsErrorName (status, NULL);
									res = 10;
								}

						}		/* if (status == 0) */
					else
						{
							error_info_s = "clientLogin";
							error_status_s = rodsErrorName (status, NULL);
							res = 10;
						}

					rcDisconnect (connection_p);
				}

		}
	else
		{
			error_info_s = "getRodsEnv";
			error_status_s = rodsErrorName (status, NULL);
			res = 10;
		}


	if (error_info_s)
		{
			if (s_verbose_flag)
				{
					printf ("%s \"%s\"\n", error_info_s, error_status_s);
				}
		}

	return res;
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

	if (len)
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
