import cv
import sys

image = 0
redtrackbar = "R"
greentrackbar = "G"
bluetrackbar = "B"
xtrackbar = "X"
ytrackbar = "Y"

imagewindow = "Image Tool: "
trackbarwindow ="Image Tool: Trackbars"

def foo(x):
    return 0

def mousecallback(mouseevent, mousex, mousey, flags, param):
    global image
    global redtrackbar
    global greentrackbar
    global bluetrackbar
    global Xtrackbar
    global Ytrackbar
    global imagewindow
    global trackbarwindow
    
    if mouseevent == cv.CV_EVENT_LBUTTONDOWN :
        scalar = image[mousey, mousex]
        cv.SetTrackbarPos(bluetrackbar, trackbarwindow, int(scalar[0]))
        cv.SetTrackbarPos(greentrackbar, trackbarwindow, int(scalar[1]))
        cv.SetTrackbarPos(redtrackbar, trackbarwindow, int(scalar[2]))
        cv.SetTrackbarPos(xtrackbar, trackbarwindow, mousex)
        cv.SetTrackbarPos(ytrackbar, trackbarwindow, mousey)

    return 0

def main() :
    global image
    global redtrackbar
    global greentrackbar
    global bluetrackbar
    global Xtrackbar
    global Ytrackbar
    global imagewindow
    global trackbarwindow
    
    if len(sys.argv) != 2 :
        print "Usage: " + sys.argv[0] +" <image>"
        sys.exit(0)

    image = cv.LoadImage(sys.argv[1], 1)
    if not image  :
        print "Unable to load image"
        sys.exit(0)

    imagewindow += sys.argv[1]
    cv.NamedWindow(imagewindow)
    cv.NamedWindow(trackbarwindow)
    cv.ShowImage(imagewindow, image)

    cv.CreateTrackbar(redtrackbar, trackbarwindow, 0, 255, foo)
    cv.CreateTrackbar(greentrackbar, trackbarwindow, 0, 255, foo)
    cv.CreateTrackbar(bluetrackbar, trackbarwindow, 0, 255, foo)
    cv.CreateTrackbar(xtrackbar, trackbarwindow, 0, image.width, foo)
    cv.CreateTrackbar(ytrackbar, trackbarwindow, 0, image.height, foo)

    cv.SetMouseCallback(imagewindow, mousecallback, 0)
    
    cv.WaitKey(0)
    cv.DestroyAllWindows()
    return 0



if __name__ == "__main__" :
    main()
