from PIL import Image



def img_to_arr(img):
    # Convert image to black and white with 8 bit pixel values
    img = img.convert("L")
    return list(img.getdata())


def arr_to_img(arr, height, width):
    print("Creating image with width {} and height {} with array size {}".format(width, height, len(arr)))
    img = Image.new("L", (int(width), int(height)))
    img.putdata(arr)
    return img


def conv_layer(arr, height, width, kernel):
    """
    Convolve an image around a kernel
    :param img: PIL Image object: The image to convolve
    :param kernel: 2D array: The kernel to convolve the image around
    :return: PIL Image object: The convolved image
    """

    ksize = len(kernel)
    # Make sure the kernel is a square. Not a rigorous check but it's enough
    if(len(kernel[0]) != ksize):
        raise Exception("Kernel must be a square")
    conv = []
    # Perform the convolution
    for p_y in range(0, height - ksize):
        for p_x in range(0, width - ksize):
            # Compute the convolution product of the kernel and the image
            conv_pix = 0
            for k_y in range(0, ksize):
                for k_x in range(0, ksize):
                    conv_pix += arr[((p_x + k_x) + (p_y + k_y)*width)] * kernel[k_y][k_x]
            conv.append(conv_pix)
    return conv


def pool_layer(arr, height, width, poolsize):
    """
    Perform max pooling on an image to downsample it
    :param img: The image to pool
    :param size: The scale factor
    :return: PIL Image object: The pooled image.
    """
    pool = []
    for p_y in range(0, height - poolsize, poolsize):
        for p_x in range(0, width - poolsize, poolsize):
            # Compute the convolution product of the kernel and the image
            largest = 0
            for k_y in range(0, poolsize):
                for k_x in range(0, poolsize):
                    if arr[((p_x + k_x) + (p_y + k_y)*width)] > largest:
                        largest = arr[(p_x + k_x + (p_y + k_y)*width)]
            pool.append(largest)
    return pool

def sigmoid(z):
    """
    Perform the sigmoid function on an input
    :param z: The number to perform sigmoid on
    :return: The result of the sigmoid function
    """
    return 1.0/(1.0+(2.71828**(-z)))


def main():
    # Basic edge detection kernel. Populate this with different values and see what happens!
    kernel = [[-1, -1, -1], [-1, 8, -1], [-1, -1, -1]]

    # Open an image and perform a convolution
    with Image.open("kitten.jpg") as raw_img:
        img_arr = img_to_arr(raw_img)
        conv_arr = conv_layer(img_arr, raw_img.height, raw_img.width, kernel)
        arr_to_img(conv_arr, raw_img.height - 3, raw_img.width - 3).show()
        pool_arr = pool_layer(conv_arr, raw_img.height - 3, raw_img.width - 3, 4)
        arr_to_img(pool_arr, 119, 79).show()


if __name__ == '__main__':
   main()