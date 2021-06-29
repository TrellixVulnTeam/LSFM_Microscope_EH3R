/**
 * @file jetraw_tiff.h
 *
 * @brief Convenience API to handle tiff files with Jetraw compression.
 * @version 1.0
 * @date 2020-06-12
 * @author Christoph Clausen (christoph.clausen@dotphoton.com)
 *
 * @copyright Copyright (c) Dotphoton AG 2020
 *
 * Christoph Clausen
 */
#pragma once

#define COMPRESSION_DOTPHOTON 48124

#ifdef _WIN32
#define CHARTYPE wchar_t
#else
#define CHARTYPE char
#endif
#include <stdint.h>
#include "dp_status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Effectively a handle to an open tiff file.
 *
 */
typedef struct dp_tiff_struct dp_tiff;

/**
 * @brief Registers Jetraw compression for libtiff.
 *
 * @return dp_status
 */
dp_status jetraw_tiff_init();

/**
 * @brief Set logging verbosity.
 * By default, log output is sent to stdout. It can be redirected to a file
 * by calling dpcore_set_logfile.
 *
 * @param level 0=Off, 1=Informational, 2=Debugging
 */
void jetraw_tiff_set_loglevel(int level);

/**
 * @brief Write log output to file instead of to stdout.
 *
 * The log output will be written to the end of the file, i.e. the file
 * is not overwritten.
 *
 * @param file_path
 * @retval dp_success if all goes well
 * @retval dp_file_write_error on failure.
 */
dp_status jetraw_tiff_set_logfile(const CHARTYPE* file_path);

/**
 * @brief Sets the license key to be used.
 *
 * If the key is set to an empty string, jetraw will look for the license
 * key in a file `license.txt`, which should be placed in the following directory,
 * - macOS: `$HOME/Library/Application Support/jetraw`
 * - Windows: `%APPDATA%\jetraw`
 *
 * @param key
 * @return dp_success or
 * @return dp_license_error if the key could not be validated.
 */
dp_status jetraw_tiff_set_license(const char* key);

/**
 *@brief Open a tiff file for writing.
 *
 *@warning If a file already exists at the given path, it will be overwritten
 *and replaced with an empty file containing only the description.
 *
 *@param[in] tiff_path The path of the file to be openend
 *@param[in] aoi_width Width of the image data that is going to be written to
 *the file. It is assumed that all the image data in a multi-page tiff will have
 *the same dimensions.
 *@param[in] aoi_height Height of the image data that is going to be written to
 *the file.
 *@param[in] tiff_description Description of the file. This goes unmodified into
 *the corresponding tiff file tag.
 *@param[out] handle A handle to the file if successfully opened.
 *@return dp_status
 *@retval #dp_success if all goes well.
 *@retval #dp_tiff_handle_in_use if the `dp_tiff` object provided represents an
 *open file.
 *@retval #dp_tiff_file_cannot_open if the file cannot be opened for writing
 *(invalid path, file locked...)
 *@retval #dp_tiff_file_update_error if the description tag of the file cannot
 *be set.
 *@retval #dp_license_error if the license could not be validated, and
 * compression is disabled.
 */
dp_status jetraw_tiff_open(const CHARTYPE* tiff_path, int aoi_width,
                           int aoi_height, const char* tiff_description,
                           dp_tiff** handle);

/**
 *@brief Compresses an image and appends it as a new image/page to an open tiff
 *file.
 *
 *@param[in] handle Handle of the file to which the image is to be appended.
 *@param[in] img_buffer Buffer that holds the uncompressed image data. Must be
 *of the dimensions supplied to #dp_open_tiff when \p handle was obtained.
 *@param[in] params Compression parameters for the camera through which the
 *image data was obtained.
 *@return dp_status
 *@retval #dp_success if the image was successfully compressed and appended to
 *the file.
 *@retval #dp_tiff_not_initialized if \p handle does not represent an open tiff
 *file.
 *@retval #dp_params_not_loaded if \p params does not represent valid
 *compression parameters.
 *@retval dp_tiff_file_update_error if the new page could not be appended to the
 *file.
 */
dp_status jetraw_tiff_append(const dp_tiff* handle, const uint16_t* img_buffer);

/**
 *@brief Closes a tiff file and frees the file handle.
 *
 *@note Once a file has been closed, no further images can be appended to it
 *through this API.
 *
 *@param[in,out] handle Handle of the file to be closed. Will be set to `null`
 *upon success.
 *@return dp_status
 *@retval dp_success if the file was closed successfully (and all data written
 *to disk).
 *@retval dp_tiff_not_initialized if \p handle does not represent an open file.
 */
dp_status jetraw_tiff_close(dp_tiff** handle);

#ifdef __cplusplus
}
#endif