from PIL import Image


def convolve_image(img, kernel):
    """
    Convolve an image around a kernel
    :param img: PIL Image object: The image to convolve
    :param kernel: 2D array: The kernel to convolve the image around
    :return: PIL Image object: The convolved image
    """
    # Convert image to black and white with 8 bit pixel values
    img = img.convert("L")
    ksize = len(kernel)
    # Make sure the kernel is a square. Not a rigorous check but it's enough
    if(len(kernel[0]) != ksize):
        raise Exception("Kernel must be a square")
    conv = []
    # Perform the convolution
    for p_y in range(0, img.height - ksize):
        for p_x in range(0, img.width - ksize):
            # Compute the convolution product of the kernel and the image
            conv_pix = 0
            for k_y in range(0, ksize):
                for k_x in range(0, ksize):
                    conv_pix += img.getpixel((p_x + k_x, p_y + k_y)) * kernel[k_y][k_x]
            conv.append(conv_pix)
    # Create a new image, fill it with convolved data, and return it
    conv_img = Image.new("L", (img.width - ksize, img.height - ksize))
    conv_img.putdata(conv)
    return conv_img


def max_pool(img, size):
    """
    Perform max pooling on an image to downsample it
    :param img: The image to pool
    :param size: The scale factor
    :return: PIL Image object: The pooled image.
    """
    img = img.convert("L")
    pool = []
    for p_y in range(0, img.height - size, size):
        for p_x in range(0, img.width - size, size):
            # Compute the convolution product of the kernel and the image
            largest = 0
            for k_y in range(0, size):
                for k_x in range(0, size):
                    if img.getpixel((p_x + k_x, p_y + k_y)) > largest:
                        largest = img.getpixel((p_x + k_x, p_y + k_y))
            pool.append(largest)
    pool_img = Image.new("L", (int(img.width/size)-1, int(img.height/size)-1))
    pool_img.putdata(pool)
    return pool_img

def main():
    # Basic edge detection kernel. Populate this with different values and see what happens!
    kernel = [[-1, -1, -1], [-1, 8, -1], [-1, -1, -1]]

    # Open an image and perform the convolution and max pool
    with Image.open("kitten.jpg") as raw_img:
        # Max pooling and convolving with edge detection kernel
        small = max_pool(raw_img, 4)
        convolve_image(small, kernel).show()

    # Open an image and perform a convolution
    with Image.open("black_kitten.jpg") as raw_img:
        convolve_image(raw_img, kernel).show()


if __name__ == '__main__':
   main()