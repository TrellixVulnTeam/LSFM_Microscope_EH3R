# -*- coding: utf-8 -*-

"""Functions for converting between color spaces.

The "central" color space in this module is RGB, more specifically the linear
sRGB color space using D65 as a white-point [1]_.  This represents a
standard monitor (w/o gamma correction). For a good FAQ on color spaces see
[2]_.

The API consists of functions to convert to and from RGB as defined above, as
well as a generic function to convert to and from any supported color space
(which is done through RGB in most cases).


Supported color spaces
----------------------
* RGB : Red Green Blue.
        Here the sRGB standard [1]_.
* HSV : Hue, Saturation, Value.
        Uniquely defined when related to sRGB [3]_.
* RGB CIE : Red Green Blue.
        The original RGB CIE standard from 1931 [4]_. Primary colors are 700 nm
        (red), 546.1 nm (blue) and 435.8 nm (green).
* XYZ CIE : XYZ
        Derived from the RGB CIE color space. Chosen such that
        ``x == y == z == 1/3`` at the whitepoint, and all color matching
        functions are greater than zero everywhere.
* LAB CIE : Lightness, a, b
        Colorspace derived from XYZ CIE that is intended to be more
        perceptually uniform
* LUV CIE : Lightness, u, v
        Colorspace derived from XYZ CIE that is intended to be more
        perceptually uniform
* LCH CIE : Lightness, Chroma, Hue
        Defined in terms of LAB CIE.  C and H are the polar representation of
        a and b.  The polar angle C is defined to be on ``(0, 2*pi)``

:author: Nicolas Pinto (rgb2hsv)
:author: Ralf Gommers (hsv2rgb)
:author: Travis Oliphant (XYZ and RGB CIE functions)
:author: Matt Terry (lab2lch)
:author: Alex Izvorski (yuv2rgb, rgb2yuv and related)

:license: modified BSD

References
----------
.. [1] Official specification of sRGB, IEC 61966-2-1:1999.
.. [2] http://www.poynton.com/ColorFAQ.html
.. [3] https://en.wikipedia.org/wiki/HSL_and_HSV
.. [4] https://en.wikipedia.org/wiki/CIE_1931_color_space
"""

import numpy as np
from warnings import warn


__all__ = ['img_as_float32', 'img_as_float64', 'img_as_float',
           'img_as_int', 'img_as_uint', 'img_as_ubyte',
           'img_as_bool', 'dtype_limits']

# For integers Numpy uses `_integer_types` basis internally, and builds a leaky
# `np.XintYY` abstraction on top of it. This leads to situations when, for
# example, there are two np.Xint64 dtypes with the same attributes but
# different object references. In order to avoid any potential issues,
# we use the basis dtypes here. For more information, see:
# - https://github.com/scikit-image/scikit-image/issues/3043
# For convenience, for these dtypes we indicate also the possible bit depths
# (some of them are platform specific). For the details, see:
# http://www.unix.org/whitepapers/64bit.html
_integer_types = (np.byte, np.ubyte,          # 8 bits
                  np.short, np.ushort,        # 16 bits
                  np.intc, np.uintc,          # 16 or 32 or 64 bits
                  int, np.int_, np.uint,      # 32 or 64 bits
                  np.longlong, np.ulonglong)  # 64 bits
_integer_ranges = {t: (np.iinfo(t).min, np.iinfo(t).max)
                   for t in _integer_types}
dtype_range = {bool: (False, True),
               np.bool_: (False, True),
               np.bool8: (False, True),
               float: (-1, 1),
               np.float_: (-1, 1),
               np.float16: (-1, 1),
               np.float32: (-1, 1),
               np.float64: (-1, 1)}
dtype_range.update(_integer_ranges)

_supported_types = list(dtype_range.keys())


def dtype_limits(image, clip_negative=False):
    """Return intensity limits, i.e. (min, max) tuple, of the image's dtype.

    Parameters
    ----------
    image : ndarray
        Input image.
    clip_negative : bool, optional
        If True, clip the negative range (i.e. return 0 for min intensity)
        even if the image dtype allows negative values.

    Returns
    -------
    imin, imax : tuple
        Lower and upper intensity limits.
    """
    imin, imax = dtype_range[image.dtype.type]
    if clip_negative:
        imin = 0
    return imin, imax


def _dtype_itemsize(itemsize, *dtypes):
    """Return first of `dtypes` with itemsize greater than `itemsize`

    Parameters
    ----------
    itemsize: int
        The data type object element size.

    Other Parameters
    ----------------
    *dtypes:
        Any Object accepted by `np.dtype` to be converted to a data
        type object

    Returns
    -------
    dtype: data type object
        First of `dtypes` with itemsize greater than `itemsize`.

    """
    return next(dt for dt in dtypes if np.dtype(dt).itemsize >= itemsize)


def _dtype_bits(kind, bits, itemsize=1):
    """Return dtype of `kind` that can store a `bits` wide unsigned int

    Parameters:
    kind: str
        Data type kind.
    bits: int
        Desired number of bits.
    itemsize: int
        The data type object element size.

    Returns
    -------
    dtype: data type object
        Data type of `kind` that can store a `bits` wide unsigned int

    """

    s = next(i for i in (itemsize, ) + (2, 4, 8) if
             bits < (i * 8) or (bits == (i * 8) and kind == 'u'))

    return np.dtype(kind + str(s))


def _scale(a, n, m, copy=True):
    """Scale an array of unsigned/positive integers from `n` to `m` bits.

    Numbers can be represented exactly only if `m` is a multiple of `n`.

    Parameters
    ----------
    a : ndarray
        Input image array.
    n : int
        Number of bits currently used to encode the values in `a`.
    m : int
        Desired number of bits to encode the values in `out`.
    copy : bool, optional
        If True, allocates and returns new array. Otherwise, modifies
        `a` in place.

    Returns
    -------
    out : array
        Output image array. Has the same kind as `a`.
    """
    kind = a.dtype.kind
    if n > m and a.max() < 2 ** m:
        mnew = int(np.ceil(m / 2) * 2)
        if mnew > m:
            dtype = "int{}".format(mnew)
        else:
            dtype = "uint{}".format(mnew)
        n = int(np.ceil(n / 2) * 2)
        warn("Downcasting {} to {} without scaling because max "
             "value {} fits in {}".format(a.dtype, dtype, a.max(), dtype),
             stacklevel=3)
        return a.astype(_dtype_bits(kind, m))
    elif n == m:
        return a.copy() if copy else a
    elif n > m:
        # downscale with precision loss
        if copy:
            b = np.empty(a.shape, _dtype_bits(kind, m))
            np.floor_divide(a, 2**(n - m), out=b, dtype=a.dtype,
                            casting='unsafe')
            return b
        else:
            a //= 2**(n - m)
            return a
    elif m % n == 0:
        # exact upscale to a multiple of `n` bits
        if copy:
            b = np.empty(a.shape, _dtype_bits(kind, m))
            np.multiply(a, (2**m - 1) // (2**n - 1), out=b, dtype=b.dtype)
            return b
        else:
            a = a.astype(_dtype_bits(kind, m, a.dtype.itemsize), copy=False)
            a *= (2**m - 1) // (2**n - 1)
            return a
    else:
        # upscale to a multiple of `n` bits,
        # then downscale with precision loss
        o = (m // n + 1) * n
        if copy:
            b = np.empty(a.shape, _dtype_bits(kind, o))
            np.multiply(a, (2**o - 1) // (2**n - 1), out=b, dtype=b.dtype)
            b //= 2**(o - m)
            return b
        else:
            a = a.astype(_dtype_bits(kind, o, a.dtype.itemsize), copy=False)
            a *= (2**o - 1) // (2**n - 1)
            a //= 2**(o - m)
            return a


def _convert(image, dtype, force_copy=False, uniform=False):
    """
    Convert an image to the requested data-type.

    Warnings are issued in case of precision loss, or when negative values
    are clipped during conversion to unsigned integer types (sign loss).

    Floating point values are expected to be normalized and will be clipped
    to the range [0.0, 1.0] or [-1.0, 1.0] when converting to unsigned or
    signed integers respectively.

    Numbers are not shifted to the negative side when converting from
    unsigned to signed integer types. Negative values will be clipped when
    converting to unsigned integers.

    Parameters
    ----------
    image : ndarray
        Input image.
    dtype : dtype
        Target data-type.
    force_copy : bool, optional
        Force a copy of the data, irrespective of its current dtype.
    uniform : bool, optional
        Uniformly quantize the floating point range to the integer range.
        By default (uniform=False) floating point values are scaled and
        rounded to the nearest integers, which minimizes back and forth
        conversion errors.

    .. versionchanged :: 0.15
        ``_convert`` no longer warns about possible precision or sign
        information loss. See discussions on these warnings at:
        https://github.com/scikit-image/scikit-image/issues/2602
        https://github.com/scikit-image/scikit-image/issues/543#issuecomment-208202228
        https://github.com/scikit-image/scikit-image/pull/3575

    References
    ----------
    .. [1] DirectX data conversion rules.
           https://msdn.microsoft.com/en-us/library/windows/desktop/dd607323%28v=vs.85%29.aspx
    .. [2] Data Conversions. In "OpenGL ES 2.0 Specification v2.0.25",
           pp 7-8. Khronos Group, 2010.
    .. [3] Proper treatment of pixels as integers. A.W. Paeth.
           In "Graphics Gems I", pp 249-256. Morgan Kaufmann, 1990.
    .. [4] Dirty Pixels. J. Blinn. In "Jim Blinn's corner: Dirty Pixels",
           pp 47-57. Morgan Kaufmann, 1998.

    """
    image = np.asarray(image)
    dtypeobj_in = image.dtype
    if dtype is np.floating:
        dtypeobj_out = np.dtype('float64')
    else:
        dtypeobj_out = np.dtype(dtype)
    dtype_in = dtypeobj_in.type
    dtype_out = dtypeobj_out.type
    kind_in = dtypeobj_in.kind
    kind_out = dtypeobj_out.kind
    itemsize_in = dtypeobj_in.itemsize
    itemsize_out = dtypeobj_out.itemsize

    # Below, we do an `issubdtype` check.  Its purpose is to find out
    # whether we can get away without doing any image conversion.  This happens
    # when:
    #
    # - the output and input dtypes are the same or
    # - when the output is specified as a type, and the input dtype
    #   is a subclass of that type (e.g. `np.floating` will allow
    #   `float32` and `float64` arrays through)

    if np.issubdtype(dtype_in, np.obj2sctype(dtype)):
        if force_copy:
            image = image.copy()
        return image

    if not (dtype_in in _supported_types and dtype_out in _supported_types):
        raise ValueError("Can not convert from {} to {}."
                         .format(dtypeobj_in, dtypeobj_out))

    if kind_in in 'ui':
        imin_in = np.iinfo(dtype_in).min
        imax_in = np.iinfo(dtype_in).max
    if kind_out in 'ui':
        imin_out = np.iinfo(dtype_out).min
        imax_out = np.iinfo(dtype_out).max

    # any -> binary
    if kind_out == 'b':
        return image > dtype_in(dtype_range[dtype_in][1] / 2)

    # binary -> any
    if kind_in == 'b':
        result = image.astype(dtype_out)
        if kind_out != 'f':
            result *= dtype_out(dtype_range[dtype_out][1])
        return result

    # float -> any
    if kind_in == 'f':
        if kind_out == 'f':
            # float -> float
            return image.astype(dtype_out)

        if np.min(image) < -1.0 or np.max(image) > 1.0:
            raise ValueError("Images of type float must be between -1 and 1.")
        # floating point -> integer
        # use float type that can represent output integer type
        computation_type = _dtype_itemsize(itemsize_out, dtype_in,
                                           np.float32, np.float64)

        if not uniform:
            if kind_out == 'u':
                image_out = np.multiply(image, imax_out,
                                        dtype=computation_type)
            else:
                image_out = np.multiply(image, (imax_out - imin_out) / 2,
                                        dtype=computation_type)
                image_out -= 1.0 / 2.
            np.rint(image_out, out=image_out)
            np.clip(image_out, imin_out, imax_out, out=image_out)
        elif kind_out == 'u':
            image_out = np.multiply(image, imax_out + 1,
                                    dtype=computation_type)
            np.clip(image_out, 0, imax_out, out=image_out)
        else:
            image_out = np.multiply(image, (imax_out - imin_out + 1.0) / 2.0,
                                    dtype=computation_type)
            np.floor(image_out, out=image_out)
            np.clip(image_out, imin_out, imax_out, out=image_out)
        return image_out.astype(dtype_out)

    # signed/unsigned int -> float
    if kind_out == 'f':
        # use float type that can exactly represent input integers
        computation_type = _dtype_itemsize(itemsize_in, dtype_out,
                                           np.float32, np.float64)

        if kind_in == 'u':
            # using np.divide or np.multiply doesn't copy the data
            # until the computation time
            image = np.multiply(image, 1. / imax_in,
                                dtype=computation_type)
            # DirectX uses this conversion also for signed ints
            # if imin_in:
            #     np.maximum(image, -1.0, out=image)
        else:
            image = np.add(image, 0.5, dtype=computation_type)
            image *= 2 / (imax_in - imin_in)

        return np.asarray(image, dtype_out)

    # unsigned int -> signed/unsigned int
    if kind_in == 'u':
        if kind_out == 'i':
            # unsigned int -> signed int
            image = _scale(image, 8 * itemsize_in, 8 * itemsize_out - 1)
            return image.view(dtype_out)
        else:
            # unsigned int -> unsigned int
            return _scale(image, 8 * itemsize_in, 8 * itemsize_out)

    # signed int -> unsigned int
    if kind_out == 'u':
        image = _scale(image, 8 * itemsize_in - 1, 8 * itemsize_out)
        result = np.empty(image.shape, dtype_out)
        np.maximum(image, 0, out=result, dtype=image.dtype, casting='unsafe')
        return result

    # signed int -> signed int
    if itemsize_in > itemsize_out:
        return _scale(image, 8 * itemsize_in - 1, 8 * itemsize_out - 1)

    image = image.astype(_dtype_bits('i', itemsize_out * 8))
    image -= imin_in
    image = _scale(image, 8 * itemsize_in, 8 * itemsize_out, copy=False)
    image += imin_out
    return image.astype(dtype_out)


def convert(image, dtype, force_copy=False, uniform=False):
    warn("The use of this function is discouraged as its behavior may change "
         "dramatically in scikit-image 1.0. This function will be removed"
         "in scikit-image 1.0.", FutureWarning, stacklevel=2)
    return _convert(image=image, dtype=dtype,
                    force_copy=force_copy, uniform=uniform)

def img_as_float32(image, force_copy=False):
    """Convert an image to single-precision (32-bit) floating point format.

    Parameters
    ----------
    image : ndarray
        Input image.
    force_copy : bool, optional
        Force a copy of the data, irrespective of its current dtype.

    Returns
    -------
    out : ndarray of float32
        Output image.

    Notes
    -----
    The range of a floating point image is [0.0, 1.0] or [-1.0, 1.0] when
    converting from unsigned or signed datatypes, respectively.
    If the input image has a float type, intensity values are not modified
    and can be outside the ranges [0.0, 1.0] or [-1.0, 1.0].

    """
    return _convert(image, np.float32, force_copy=force_copy)


def img_as_float64(image, force_copy=False):
    """Convert an image to double-precision (64-bit) floating point format.

    Parameters
    ----------
    image : ndarray
        Input image.
    force_copy : bool, optional
        Force a copy of the data, irrespective of its current dtype.

    Returns
    -------
    out : ndarray of float64
        Output image.

    Notes
    -----
    The range of a floating point image is [0.0, 1.0] or [-1.0, 1.0] when
    converting from unsigned or signed datatypes, respectively.
    If the input image has a float type, intensity values are not modified
    and can be outside the ranges [0.0, 1.0] or [-1.0, 1.0].

    """
    return _convert(image, np.float64, force_copy=force_copy)


def img_as_float(image, force_copy=False):
    """Convert an image to floating point format.

    This function is similar to `img_as_float64`, but will not convert
    lower-precision floating point arrays to `float64`.

    Parameters
    ----------
    image : ndarray
        Input image.
    force_copy : bool, optional
        Force a copy of the data, irrespective of its current dtype.

    Returns
    -------
    out : ndarray of float
        Output image.

    Notes
    -----
    The range of a floating point image is [0.0, 1.0] or [-1.0, 1.0] when
    converting from unsigned or signed datatypes, respectively.
    If the input image has a float type, intensity values are not modified
    and can be outside the ranges [0.0, 1.0] or [-1.0, 1.0].

    """
    return _convert(image, np.floating, force_copy=force_copy)


def img_as_uint(image, force_copy=False):
    """Convert an image to 16-bit unsigned integer format.

    Parameters
    ----------
    image : ndarray
        Input image.
    force_copy : bool, optional
        Force a copy of the data, irrespective of its current dtype.

    Returns
    -------
    out : ndarray of uint16
        Output image.

    Notes
    -----
    Negative input values will be clipped.
    Positive values are scaled between 0 and 65535.

    """
    return _convert(image, np.uint16, force_copy=force_copy)


def img_as_int(image, force_copy=False):
    """Convert an image to 16-bit signed integer format.

    Parameters
    ----------
    image : ndarray
        Input image.
    force_copy : bool, optional
        Force a copy of the data, irrespective of its current dtype.

    Returns
    -------
    out : ndarray of int16
        Output image.

    Notes
    -----
    The values are scaled between -32768 and 32767.
    If the input data-type is positive-only (e.g., uint8), then
    the output image will still only have positive values.

    """
    return _convert(image, np.int16, force_copy=force_copy)


def img_as_ubyte(image, force_copy=False):
    """Convert an image to 8-bit unsigned integer format.

    Parameters
    ----------
    image : ndarray
        Input image.
    force_copy : bool, optional
        Force a copy of the data, irrespective of its current dtype.

    Returns
    -------
    out : ndarray of ubyte (uint8)
        Output image.

    Notes
    -----
    Negative input values will be clipped.
    Positive values are scaled between 0 and 255.

    """
    return _convert(image, np.uint8, force_copy=force_copy)


def img_as_bool(image, force_copy=False):
    """Convert an image to boolean format.

    Parameters
    ----------
    image : ndarray
        Input image.
    force_copy : bool, optional
        Force a copy of the data, irrespective of its current dtype.

    Returns
    -------
    out : ndarray of bool (`bool_`)
        Output image.

    Notes
    -----
    The upper half of the input dtype's positive range is True, and the lower
    half is False. All negative values (if present) are False.

    """
    return _convert(image, bool, force_copy=force_copy)


def _prepare_colorarray(arr, force_copy=False):
    """Check the shape of the array and convert it to
    floating point representation.
    """
    arr = np.asanyarray(arr)

    if arr.shape[-1] != 3:
        raise ValueError("Input array must have a shape == (..., 3)), "
                         f"got {arr.shape}")

    return img_as_float(arr, force_copy=force_copy)


def rgba2rgb(rgba, background=(1, 1, 1)):
    """RGBA to RGB conversion using alpha blending [1]_.

    Parameters
    ----------
    rgba : (..., 4) array_like
        The image in RGBA format. Final dimension denotes channels.
    background : array_like
        The color of the background to blend the image with (3 floats
        between 0 to 1 - the RGB value of the background).

    Returns
    -------
    out : (..., 3) ndarray
        The image in RGB format. Same dimensions as input.

    Raises
    ------
    ValueError
        If `rgba` is not at least 2-D with shape (..., 4).

    References
    ----------
    .. [1] https://en.wikipedia.org/wiki/Alpha_compositing#Alpha_blending

    Examples
    --------
    >>> from skimage import color
    >>> from skimage import data
    >>> img_rgba = data.logo()
    >>> img_rgb = color.rgba2rgb(img_rgba)
    """
    arr = np.asanyarray(rgba)

    if arr.shape[-1] != 4:
        msg = ("the input array must have shape == (..., 4)), "
               f"got {arr.shape}")
        raise ValueError(msg)

    arr = img_as_float(arr)

    background = np.ravel(background).astype(arr.dtype)
    if len(background) != 3:
        raise ValueError('background must be an array-like containing 3 RGB '
                         f'values. Got {len(background)} items')
    if np.any(background < 0) or np.any(background > 1):
        raise ValueError('background RGB values must be floats between '
                         '0 and 1.')

    background = background[np.newaxis, ...]
    alpha = arr[..., -1, np.newaxis]
    channels = arr[np.newaxis, ..., :-1]

    out = np.squeeze(np.clip((1 - alpha) * background + alpha * channels,
                             a_min=0, a_max=1),
                     axis=0)
    return out


def rgb2hsv(rgb):
    """RGB to HSV color space conversion.

    Parameters
    ----------
    rgb : (..., 3) array_like
        The image in RGB format. Final dimension denotes channels.

    Returns
    -------
    out : (..., 3) ndarray
        The image in HSV format. Same dimensions as input.

    Raises
    ------
    ValueError
        If `rgb` is not at least 2-D with shape (..., 3).

    Notes
    -----
    Conversion between RGB and HSV color spaces results in some loss of
    precision, due to integer arithmetic and rounding [1]_.

    References
    ----------
    .. [1] https://en.wikipedia.org/wiki/HSL_and_HSV

    Examples
    --------
    >>> from skimage import color
    >>> from skimage import data
    >>> img = data.astronaut()
    >>> img_hsv = color.rgb2hsv(img)
    """
    input_is_one_pixel = rgb.ndim == 1
    if input_is_one_pixel:
        rgb = rgb[np.newaxis, ...]

    arr = _prepare_colorarray(rgb)
    out = np.empty_like(arr)

    # -- V channel
    out_v = arr.max(-1)

    # -- S channel
    delta = arr.ptp(-1)
    # Ignore warning for zero divided by zero
    old_settings = np.seterr(invalid='ignore')
    out_s = delta / out_v
    out_s[delta == 0.] = 0.

    # -- H channel
    # red is max
    idx = (arr[..., 0] == out_v)
    out[idx, 0] = (arr[idx, 1] - arr[idx, 2]) / delta[idx]

    # green is max
    idx = (arr[..., 1] == out_v)
    out[idx, 0] = 2. + (arr[idx, 2] - arr[idx, 0]) / delta[idx]

    # blue is max
    idx = (arr[..., 2] == out_v)
    out[idx, 0] = 4. + (arr[idx, 0] - arr[idx, 1]) / delta[idx]
    out_h = (out[..., 0] / 6.) % 1.
    out_h[delta == 0.] = 0.

    np.seterr(**old_settings)

    # -- output
    out[..., 0] = out_h
    out[..., 1] = out_s
    out[..., 2] = out_v

    # # remove NaN
    out[np.isnan(out)] = 0

    if input_is_one_pixel:
        out = np.squeeze(out, axis=0)

    return out


def hsv2rgb(hsv):
    """HSV to RGB color space conversion.

    Parameters
    ----------
    hsv : (..., 3) array_like
        The image in HSV format. Final dimension denotes channels.

    Returns
    -------
    out : (..., 3) ndarray
        The image in RGB format. Same dimensions as input.

    Raises
    ------
    ValueError
        If `hsv` is not at least 2-D with shape (..., 3).

    Notes
    -----
    Conversion between RGB and HSV color spaces results in some loss of
    precision, due to integer arithmetic and rounding [1]_.

    References
    ----------
    .. [1] https://en.wikipedia.org/wiki/HSL_and_HSV

    Examples
    --------
    >>> from skimage import data
    >>> img = data.astronaut()
    >>> img_hsv = rgb2hsv(img)
    >>> img_rgb = hsv2rgb(img_hsv)
    """
    arr = _prepare_colorarray(hsv)

    hi = np.floor(arr[..., 0] * 6)
    f = arr[..., 0] * 6 - hi
    p = arr[..., 2] * (1 - arr[..., 1])
    q = arr[..., 2] * (1 - f * arr[..., 1])
    t = arr[..., 2] * (1 - (1 - f) * arr[..., 1])
    v = arr[..., 2]

    hi = np.stack([hi, hi, hi], axis=-1).astype(np.uint8) % 6
    out = np.choose(
        hi, np.stack([np.stack((v, t, p), axis=-1),
                      np.stack((q, v, p), axis=-1),
                      np.stack((p, v, t), axis=-1),
                      np.stack((p, q, v), axis=-1),
                      np.stack((t, p, v), axis=-1),
                      np.stack((v, p, q), axis=-1)]))

    return out