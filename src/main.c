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


static bool s_debug_flag = false;


static const char *GetLocalName (const char *path_s);
static int GetCollectionEntries (rcComm_t *connection_p, char *path_s, const char **error_info_ss, const char **error_status_ss);



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
			rcComm_t *connection_p = rcConnect (env.rodsHost, env.rodsPort, env.rodsUserName, env.rodsZone,	0, &err);

			if (connection_p)
				{
					status = clientLogin (connection_p, NULL, NULL);

					if (status == 0)
						{
							collHandle_t coll_handle;
							char *input_path_s = (char *) argv [1];
							int flags = DATA_QUERY_FIRST_FG;

							status = GetCollectionEntries (connection_p, input_path_s, &error_info_s, &error_status_s);

							// Open the collection.
							if (s_debug_flag)
								{
									printf ("Opening \"%s\"\n", input_path_s);
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
			if (s_debug_flag)
				{
					printf ("%s \"%s\"\n", error_info_s, error_status_s);
				}
		}

	return res;
}


static int GetCollectionEntries (rcComm_t *connection_p, char *path_s, const char **error_info_ss, const char **error_status_ss)
{
	collHandle_t coll_handle;
	int status;
	int res = 0;

	memset (&coll_handle, 0, sizeof (collHandle_t));

	status = rclOpenCollection (connection_p, path_s, DATA_QUERY_FIRST_FG, &coll_handle);

	if (status == 0)
		{
			ByteBuffer *buffer_p = AllocateByteBuffer (1024);

			if (buffer_p)
				{
					collEnt_t coll_entry;
					size_t index = 0;

					do
						{
							status = rclReadCollection (connection_p, &coll_handle, &coll_entry);

							if (status < 0)
								{
									if (status != CAT_NO_ROWS_FOUND)
										{
											/* Failed to read collection */
											*error_info_ss = "rclReadCollection";
											*error_status_ss = rodsErrorName (status, NULL);

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
			*error_info_ss = "rclOpenCollection";
			*error_status_ss = rodsErrorName (status, NULL);

			res = 10;
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
