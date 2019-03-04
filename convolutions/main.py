from PIL import Image

KSIZE = 3

with Image.open("kitten.jpg") as raw_img:
    # Convert image to black and white with 8 bit color values
    img = raw_img.convert("L")
    # Instantiate the kernel for the convolution
    kernel = [[-1, -1, -1], [-1, 8, -1], [-1, -1, -1]]
    # Instantiate array for convolved image
    conv = []
    for p_y in range(0, img.height - KSIZE):
        for p_x in range(0, img.width - KSIZE):
            # Compute the convolution product of the kernel and the image
            conv_pix = 0
            for k_y in range(0, KSIZE):
                for k_x in range(0, KSIZE):
                    conv_pix += img.getpixel((p_x + k_x, p_y + k_y)) * kernel[k_y][k_x]
            conv.append(conv_pix)
    print(conv)
    conv_img = Image.new("L", (img.width - KSIZE, img.height - KSIZE))
    conv_img.putdata(conv)
    print(conv_img.width)
    print(conv_img.height)
    conv_img.show()
    img.show()