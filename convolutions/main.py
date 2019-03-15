from PIL import Image
import math
import random

#S2 is not fully connected to C3, it is connected based on this mapping
c3_connection_map = [[1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1],
                     [1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1],
                     [1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1],
                     [0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 1, 1],
                     [0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1]]

class LeNet5:
    # Python3 PIL image object
    input_img = None

    # Image pixel data, as an array
    input_img_data = []

    # First layer: Convolutions. 6 feature maps at 28x28
    c1_feature_maps = []
    # Second layer: Max Pooling. 6 feature maps at 14x14
    s2_feature_maps = []
    # Third layer: Convolutions. 16 feature maps at 10x10. NOT fully connected to s2
    c3_feature_maps = []
    # Fourth layer: Max pooling: 16 feature maps at 5x5
    s4_feature_maps = []
    # Fifth layer: Convolutions. 120 feature maps of 1x1
    c5_feature_maps = []
    # 6th layer: Fully connected layer. 84 sigmoid neurons
    f6_connected_layer = []
    # 7th layer: Output. 10 sigmoid neurons
    o7_output_layer = []

    # Final verdict for the fed in image
    verdict = 0

    def __init__(self, img):
        # Take in an image for analysis
        self.input_img = img
        # Convert the image into black and white 8 bit data
        self.input_img_data = img_to_arr(self.input_img)
        # Initialize all the layers with random weights
        # Initialize the C1 layer
        for i in range(0, 6):
            self.c1_feature_maps.append(ConvLayer(self.input_img.height,
                                                  self.input_img.width,
                                                  4))
        # Initialize the S2 Layer
        s2_height = self.input_img.height - self.c1_feature_maps[0].get_kernel_size()
        s2_width = self.input_img.width - self.c1_feature_maps[0].get_kernel_size()
        for i in range(0, 6):
            self.s2_feature_maps.append(MaxPoolLayer(2,
                                                     s2_height,
                                                     s2_width))

        # Initialize the C3 Layer
        c3_height = math.floor(s2_height/self.s2_feature_maps[0].get_poolsize())
        c3_width = math.floor(s2_width/self.s2_feature_maps[0].get_poolsize())
        for i in range(0, 16):
            self.c3_feature_maps.append(ConvLayer(c3_height,
                                                  c3_width,
                                                  4))
        # Initialize the S4 Layer
        s4_height = c3_height - self.c3_feature_maps[0].get_kernel_size()
        s4_width = c3_width - self.c3_feature_maps[0].get_kernel_size()
        for i in range(0, 16):
            self.s4_feature_maps.append(MaxPoolLayer(2,
                                                     s4_height,
                                                     s4_width))

        self.c5_feature_maps = [[] for i in range(120)]
        self.f6_connected_layer = [[] for i in range(84)]
        self.o7_output_layer = [[] for i in range(10)]

    def feed_forward(self):
        """
        Take the image in the network, feed it through the network, and set the final verdict
        :return:
        """


class ConvLayer:
    # The kernel for the convolutional layer
    kernel = []
    # The height of the input data
    height = 0
    # The width of the input data
    width = 0
    # The height/width of the square kernel
    ksize = 0

    def __init__(self, height, width, ksize):
        self.height = height
        self.width = width
        self.ksize = ksize
        # Initialize a random square kernel
        for i in range(0, ksize**2):
            self.kernel.append(random.gauss(0, 1))

    def convolve(self, data):
        """
        Convolve an image around a kernel
        :return: array: The convolved data
        """
        conv = []
        # Perform the convolution
        for p_y in range(0, self.height - self.ksize):
            for p_x in range(0, self.width - self.ksize):
                # Compute the convolution product of the kernel and the image
                conv_pix = 0
                for k_y in range(0, self.ksize):
                    for k_x in range(0, self.ksize):
                        conv_pix += data[((p_x + k_x) + (p_y + k_y) * self.width)] * self.kernel[(k_y * self.ksize) + k_x]
                conv.append(conv_pix)
        return conv

    def get_kernel(self):
        """
        Get the kernel (weight) used
        :return: array: The Kernel
        """
        return self.kernel

    def get_kernel_size(self):
        return self.ksize

    def set_kernel(self, kernel):
        """
        Set the kernel after back propogation has been performed
        :return: nothing
        """
        self.kernel = kernel

class MaxPoolLayer:
    # The amount that pooling will downsample the image
    pool_size = 0
    # The height of the input data
    height = 0
    # The width of the input data
    width = 0

    def __init__(self, pool_size, height, width):
        self.pool_size = pool_size
        self.height = height
        self.width = width

    def max_pool(self, data):
        pool = []
        for p_y in range(0, self.height - self.pool_size, self.pool_size):
            for p_x in range(0, self.width - self.pool_size, self.pool_size):
                # Compute the convolution product of the kernel and the image
                largest = 0
                for k_y in range(0, self.pool_size):
                    for k_x in range(0, self.pool_size):
                        if data[((p_x + k_x) + (p_y + k_y) * self.width)] > largest:
                            largest = data[(p_x + k_x + (p_y + k_y) * self.width)]
                pool.append(largest)
        return pool

    def get_pool_size(self):
        return self.pool_size

def img_to_arr(img):
    # Convert image to black and white with 8 bit pixel values
    img = img.convert("L")
    return list(img.getdata())


def arr_to_img(arr, height, width):
    print("Creating image with width {} and height {} with array size {}".format(width, height, len(arr)))
    img = Image.new("L", (int(width), int(height)))
    img.putdata(arr)
    return img

def sigmoid(z):
    """
    Perform the sigmoid function on an input
    :param z: The number to perform sigmoid on
    :return: The result of the sigmoid function
    """
    return 1.0/(1.0+(math.e**(-z)))


def main():
    # Basic edge detection kernel. Populate this with different values and see what happens!
    kernel = [-1, -1, -1, -1, 8, -1, -1, -1, -1]

    # Open an image and perform a convolution
    with Image.open("kitten.jpg") as raw_img:
        img_arr = img_to_arr(raw_img)
        conv_layer = ConvLayer(raw_img.height, raw_img.width, 3)
        arr_to_img(conv_layer.convolve(img_arr), raw_img.height - 3, raw_img.width - 3).show()
        conv_layer.set_kernel(kernel)
        convolved = conv_layer.convolve(img_arr)
        arr_to_img(convolved, raw_img.height - 3, raw_img.width - 3).show()
        pool_layer = MaxPoolLayer(4, raw_img.height - 3, raw_img.width - 3)
        arr_to_img(pool_layer.max_pool(convolved), 119, 79).show()

if __name__ == '__main__':
   main()