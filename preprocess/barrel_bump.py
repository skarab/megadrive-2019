import numpy
import cv2

source = cv2.imread('barrel_bump.png', 1)
source_height = cv2.imread('barrel_bump_height.png', 1)

kernel = numpy.ones((3,3),numpy.float32)/9
source_height = cv2.filter2D(source_height,-1,kernel)

height, width, depth = source.shape

output = numpy.zeros((402, 256, 3), numpy.uint8)

x = 0
y = 0
w = 128
while w>=102:
    b = (w-100.0)/28.0
    h = height*w//width
    img = cv2.resize(source, (w, h))
    img_height = cv2.resize(source_height, (w, h))
    
    m = numpy.array([b,b,b]).reshape((1,3))
    img = cv2.multiply(img, m)
    
    bump = 1.0-b
    mapx = numpy.zeros((h, w), numpy.float32)
    mapy = numpy.zeros((h, w), numpy.float32)
    
    for my in range(0, h):
        for mx in range(0, w):
            mapx[my, mx] = mx
            mapy[my, mx] = my-(255-img_height[my, mx, 0])*bump*80.0/255.0
    
    output[y:y+h, x:x+w] = cv2.remap(img, mapx, mapy, cv2.INTER_LINEAR) #, flags=cv2.CV_INTER_LINEAR+cv2.CV_WARP_FILL_OUTLIERS, fillval=(0, 0, 0, 0))
    
    for my in range(0, h):
        for mx in range(0, w):
            mapx[my, mx] = mx
            mapy[my, mx] = my+(255-img_height[my, mx, 0])*bump*80.0/255.0
    
    output[y:y+h, x+128:x+w+128] = cv2.remap(img, mapx, mapy, cv2.INTER_LINEAR) #, flags=cv2.CV_INTER_LINEAR+cv2.CV_WARP_FILL_OUTLIERS, fillval=(0, 0, 0, 0))
        
    w = w - 2
    x = x + 1
    y = y + h

cv2.imwrite('barrel_bump_output.png', output)
    