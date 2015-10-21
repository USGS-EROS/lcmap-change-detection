/*****************************************************************************
!File: input.c
*****************************************************************************/

#include "input.h"
#include "utilities.h"

const char raw_binary_format[][4] = {"rb", "wb", "rb+"};

FILE *open_raw_binary
(
    char *infile,        /* I: name of the input file to be opened */
    char *access_type    /* I: string for the access type for reading the
                               input file; use the raw_binary_format
                               array at the top of this file */
)
{
    FILE *rb_fptr = NULL;    /* pointer to the raw binary file */
    char FUNC_NAME[] = "open_raw_binary"; /* function name */

    /* Open the file with the specified access type */
    rb_fptr = fopen (infile, access_type);
    if (rb_fptr == NULL)
    {
         ERROR_MESSAGE("Opening raw binary", FUNC_NAME);
	 return NULL;
    }

    /* Return the file pointer */
    return rb_fptr;
}

void close_raw_binary
(
    FILE *fptr      /* I: pointer to raw binary file to be closed */
)
{
    fclose (fptr);
}

int write_raw_binary
(
    FILE *rb_fptr,      /* I: pointer to the raw binary file */
    int nlines,         /* I: number of lines to write to the file */
    int nsamps,         /* I: number of samples to write to the file */
    int size,           /* I: number of bytes per pixel (ex. sizeof(uint8)) */
    void *img_array     /* I: array of nlines * nsamps * size to be written
                              to the raw binary file */
)
{
    int nvals;               /* number of values written to the file */
    char FUNC_NAME[] = "write_raw_binary"; /* function name */

    /* Write the data to the raw binary file */
    nvals = fwrite (img_array, size, nlines * nsamps, rb_fptr);
    if (nvals != nlines * nsamps)
    {
        RETURN_ERROR("Incorrect amount of data written", FUNC_NAME, ERROR);
    }

    return SUCCESS;
}

int read_raw_binary
(
    FILE *rb_fptr,      /* I: pointer to the raw binary file */
    int nlines,         /* I: number of lines to read from the file */
    int nsamps,         /* I: number of samples to read from the file */
    int size,           /* I: number of bytes per pixel (ex. sizeof(uint8)) */
    void *img_array     /* O: array of nlines * nsamps * size to be read from
                              the raw binary file (sufficient space should
                              already have been allocated) */
)
{
    int nvals;               /* number of values read from the file */
    char FUNC_NAME[] = "read_raw_binary"; /* function name */

    /* Read the data from the raw binary file */
    nvals = fread (img_array, size, nlines * nsamps, rb_fptr);
    if (nvals != nlines * nsamps)
    {
        RETURN_ERROR("Incorrect amount of data read", FUNC_NAME, ERROR);
    }

    return SUCCESS;
}

/******************************************************************************
MODULE: trimwhitespace

PURPOSE: Trim leading spaces of a sting
 
RETURN VALUE:
Type = string without trailing space

HISTORY:
Date         Programmer       Reason
----------   --------------   -------------------------------------
1/16/2015    Song Guo         Modified from online code

NOTES:
*****************************************************************************/
char *trimwhitespace(char *str)
{
  char *end;

  /* Trim leading space */
  while(isspace(*str)) str++;

  if(*str == 0)  
    return str;

  /* Trim trailing space */
  end = str + strlen(str) - 1;
  while(end > str && isspace(*end)) end--;

  /* Write new null terminator */
  *(end+1) = 0;

  return str;
}

/******************************************************************************
MODULE: read_envi_header

PURPOSE: Reads envi header info into input structure
 
RETURN VALUE:
Type = None

HISTORY:
Date         Programmer       Reason
----------   --------------   -------------------------------------
1/16/2015    Song Guo         Original development

NOTES:
*****************************************************************************/
int read_envi_header
(
    char *scene_name,      /* I: scene name*/
    Input_meta_t *meta     /* O: saved header file info */
)
{
    char  buffer[MAX_STR_LEN] = "\0";
    char  *label = NULL;
    char  *tokenptr = NULL;
    char  *tokenptr2 = NULL;
    char  *seperator = "=";
    char  *seperator2 = ",";
    FILE *in;
    int ib;
    char map_info[10][MAX_STR_LEN]; 
    char filename[MAX_STR_LEN];
    char FUNC_NAME[] = "read_envi_header"; /* function name */

    //    sprintf(filename, "%s_sr_band1.hdr", scene_name);
    sprintf(filename, "%s.hdr", scene_name);

    in=fopen(filename, "r");
    if (in == NULL)
    {
        RETURN_ERROR ("opening header file", FUNC_NAME, FAILURE);
    }

    /* process line by line */
    while(fgets(buffer, MAX_STR_LEN, in) != NULL) 
    {

        char *s;
        s = strchr(buffer, '=');
        if (s != NULL)
        {
            /* get string token */
            tokenptr = strtok(buffer, seperator);
            label=trimwhitespace(tokenptr);

            if (strcmp(label,"lines") == 0)
            {
                tokenptr = trimwhitespace(strtok(NULL, seperator));
                meta->lines = atoi(tokenptr);
            }

            if (strcmp(label,"data type") == 0)
            {
                tokenptr = trimwhitespace(strtok(NULL, seperator));
                meta->data_type = atoi(tokenptr);
            }

            if (strcmp(label,"byte order") == 0)
            {
                tokenptr = trimwhitespace(strtok(NULL, seperator));
                meta->byte_order = atoi(tokenptr);
            }

            if (strcmp(label,"samples") == 0)
            {
                tokenptr = trimwhitespace(strtok(NULL, seperator));
                meta->samples = atoi(tokenptr);
            }

            if (strcmp(label,"interleave") == 0)
            {
                tokenptr = trimwhitespace(strtok(NULL, seperator));
                strcpy(meta->interleave, tokenptr);
            }

            if (strcmp(label,"UPPER_LEFT_CORNER") == 0)
            {
                tokenptr = trimwhitespace(strtok(NULL, seperator));
            }

            if (strcmp(label,"map info") == 0)
            {
                tokenptr = trimwhitespace(strtok(NULL, seperator));
            }

            if (strcmp(label,"map info") == 0)
            {
                tokenptr2 = strtok(tokenptr, seperator2);
                ib = 0;
                while(tokenptr2 != NULL)
                {
                    strcpy(map_info[ib], tokenptr2);
                    if (ib == 3)
                        meta->upper_left_x = atoi(map_info[ib]);
                    if (ib == 4)
                        meta->upper_left_y = atoi(map_info[ib]);
                    if (ib == 5)
                        meta->pixel_size = atoi(map_info[ib]);
                    if(ib == 7)
                        meta->utm_zone = atoi(map_info[ib]);
                    tokenptr2 = strtok(NULL, seperator2);
                    ib++;
                }
            }
        }
    }
    fclose(in);

    return SUCCESS;
}
