/*
 * main.c
 *
 *  Created on: 4 Apr 2018
 *      Author: billy
 */

#include <stdio.h>
#include <string.h>

#include "rodsClient.h"

#include "byte_buffer.h"

static const char *GetLocalName (const char *path_s);

static bool s_debug_flag = false;


int main (int argc, char *argv [])
{
	int res = 0;
	rodsEnv env;
	int status = getRodsEnv (&env);
	const char *error_info_s = NULL;
	const char *error_status_s = NULL;

	if (status >= 0)
		{
			rErrMsg_t err;
			rcComm_t *conn_p = rcConnect (env.rodsHost, env.rodsPort, env.rodsUserName, env.rodsZone,	0, &err);

			if (conn_p)
				{
					status = clientLogin (conn_p, NULL, NULL);

					if (status == 0)
						{
							collHandle_t coll_handle;
							collInp_t input_path;
							char *input_path_s = (char *) argv [1];
							int flags = DATA_QUERY_FIRST_FG;

							memset (&input_path, 0, sizeof (collInp_t));
							strcpy (input_path.collName, argv [1]);

							memset (&coll_handle, 0, sizeof (collHandle_t));

							// Open the collection.
							if (s_debug_flag)
								{
									printf ("Opening \"%s\"\n", input_path_s);
								}

							status = rclOpenCollection (conn_p, input_path_s, flags, &coll_handle);

							if (status == 0)
								{
									ByteBuffer *buffer_p = AllocateByteBuffer (1024);

									if (buffer_p)
										{
											collEnt_t coll_entry;
											size_t index = 0;

											do
												{
													status = rclReadCollection (conn_p, &coll_handle, &coll_entry);

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
															const char *name_s = (coll_entry.objType == DATA_OBJ_T) ? coll_entry.dataName : GetLocalName (coll_entry.collName);

															if (s_debug_flag)
																{
																	printf ("[%lu]: \"%s\"\n", index, name_s);
																}

															if (index > 0)
																{
																	AppendStringsToByteBuffer (buffer_p, " ", name_s, NULL);
																}
															else
																{
																	AppendStringsToByteBuffer (buffer_p, name_s, NULL);
																}

															++ index;
														}

												}
											while (status >= 0);

											if (GetByteBufferSize (buffer_p))
												{
													char *data_s = DetachByteBufferData (buffer_p);

													printf ("%s\n", data_s);
												}
										}		/* if (buffer_p) */
								}
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

					rcDisconnect (conn_p);
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
			if (s_debug_flag)
				{
					printf ("%s \"%s\"\n", error_info_s, error_status_s);
				}
		}

	return res;
}




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
							res_s = ptr;
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
