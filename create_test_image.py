import numpy as np
import cv2

# Create a black image
width, height = 256, 256
image = np.zeros((height, width, 3), np.uint8)

# Draw a red square in the middle
cv2.rectangle(image, (64, 64), (192, 192), (0, 0, 255), -1)

# Save the image
cv2.imwrite("test_image.png", image)
