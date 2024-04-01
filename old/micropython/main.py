from machine import I2C, Pin
from ssd1306 import SSD1306_I2C
import ujson as json
import sys
import gfx
import utime

# Top (Yellow): 16 x 128 / Bottom (Blue): 48 x 128 | Full: 64 x 128
pix_res_x = 128  # SSD1306 horizontal resolution
pix_res_y = 64   # SSD1306 vertical resolution

# start I2C on I2C1 (GPIO 26/27)
i2c_dev = I2C(1, scl=Pin(27), sda=Pin(26), freq=400000)
i2c_addr = [hex(ii) for ii in i2c_dev.scan()]  # get I2C address in hex format
if i2c_addr == []:
    print('No I2C Display Found')
    sys.exit()  # exit routine if no dev found
else:
    print("I2C Address      : {}".format(i2c_addr[0]))  # I2C device address
    print("I2C Configuration: {}".format(i2c_dev))  # print I2C params

oled = SSD1306_I2C(pix_res_x, pix_res_y, i2c_dev)  # oled controller
graphics = gfx.GFX(pix_res_x, pix_res_y, oled.pixel)

upIN = 0
downIN = 1
leftIN = 2
rightIN = 3

upOUT = 4
downOUT = 5
leftOUT = 6
rightOUT = 7

btnUpIN = Pin(upIN, Pin.IN, Pin.PULL_UP)
btnDownIN = Pin(downIN, Pin.IN, Pin.PULL_UP)
btnLeftIN = Pin(leftIN, Pin.IN, Pin.PULL_UP)
btnRightIN = Pin(rightIN, Pin.IN, Pin.PULL_UP)

# outputs start as inputs for safety
btnUpOUT = Pin(upOUT, Pin.IN)
btnDownOUT = Pin(downOUT, Pin.IN)
btnLeftOUT = Pin(leftOUT, Pin.IN)
btnRightOUT = Pin(rightOUT, Pin.IN)

btnSquare = Pin(8, Pin.IN, Pin.PULL_UP)
btnTriangle = Pin(9, Pin.IN, Pin.PULL_UP)
btnR1 = Pin(10, Pin.IN, Pin.PULL_UP)
btnL1 = Pin(11, Pin.IN, Pin.PULL_UP)
btnX = Pin(12, Pin.IN, Pin.PULL_UP)
btnCircle = Pin(13, Pin.IN, Pin.PULL_UP)
btnR2 = Pin(14, Pin.IN, Pin.PULL_UP)
btnL2 = Pin(15, Pin.IN, Pin.PULL_UP)

btnHome = Pin(16, Pin.IN, Pin.PULL_UP)
btnShare = Pin(17, Pin.IN, Pin.PULL_UP)
btnOptions = Pin(18, Pin.IN, Pin.PULL_UP)
# R3 & L3 are active Highs, opposite of the rest
btnR3 = Pin(19, Pin.IN, Pin.PULL_DOWN)
# R3 & L3 are active Highs, opposite of the rest
btnL3 = Pin(20, Pin.IN, Pin.PULL_DOWN)
btnTouch = Pin(21, Pin.IN, Pin.PULL_UP)

btnChooseSOCD = Pin(22, Pin.IN, Pin.PULL_UP)

upFlag = True
downFlag = True
leftFlag = True
rightFlag = True
squareFlag = True
triangleFlag = True
xFlag = True
circleFlag = True
r1Flag = True
r2Flag = True
r3Flag = True
l1Flag = True
l2Flag = True
l3Flag = True
homeFlag = True
shareFlag = True
optionsFlag = True
touchFlag = True
SOCDChangeFlag = True

upFlag2 = False
downFlag2 = False
leftFlag2 = False
rightFlag2 = False
squareFlag2 = False
triangleFlag2 = False
xFlag2 = False
circleFlag2 = False
r1Flag2 = False
r2Flag2 = False
r3Flag2 = False
l1Flag2 = False
l2Flag2 = False
l3Flag2 = False
homeFlag2 = False
shareFlag2 = False
optionsFlag2 = False
touchFlag2 = False
SOCDChangeFlag2 = False

currentMillis = 0
upPreviousMillis = 0
downPreviousMillis = 0
leftPreviousMillis = 0
rightPreviousMillis = 0

upPreviousState = 0
downPreviousState = 0
leftPreviousState = 0
rightPreviousState = 0

SOCD_mode = None
SOCD_speech_text = ' '
SOCD_last_value = ' '
art_animation_speed = 43
art_animation_control = art_animation_speed
art_animation_flag = True

upRead = btnUpIN.value()
downRead = btnDownIN.value()
leftRead = btnLeftIN.value()
rightRead = btnRightIN.value()

##### Read SOCD from inputs or saved file #####
# save SOCD state to flash / JSON file
jsonData = {'SOCD': SOCD_mode}


def save_SOCD_state(state):
    jsonData['SOCD'] = state
    try:
        with open('savedata.json', 'w') as f:
            json.dump(jsonData, f)
    except:
        print('Could not save the SOCD state variable')


##### SOCD MODES #####


def drawSpeechText(clear=None):
    def drawBubble(color):
        gLine(5, 35, 38, 35, color)
        gLine(4, 36, 4, 44, color)
        gLine(39, 36, 39, 44, color)
        gLine(5, 45, 38, 45, color)
        gTriangle(15, 45, 14, 52, 25, 45, color)
        gLine(16, 45, 24, 45, 0)
        gPixel(15, 52, color)

    if clear == 'clear':
        drawBubble(0)
        graphics.fill_rect(6, 37, 32, 7, 0)
    else:
        splitText = [x for x in SOCD_speech_text]
        x_pos = 5
        loop_count = 0
        drawBubble(1)

        for x in splitText:
            gText(x, x_pos, 37)
            if loop_count >= 2:
                x_pos += 6
            else:
                x_pos += 7

            loop_count += 1


def useSOCD0():  # SOCD-N: left + right = neutral; up + down = neutral
    global btnUpOUT
    global btnDownOUT
    global btnLeftOUT
    global btnRightOUT
    global SOCD_speech_text
    global SOCD_last_value

    if leftRead == False:  # button is pressed
        if rightRead:  # button is not pressed
            btnRightOUT = Pin(rightOUT, Pin.IN)
            btnLeftOUT = Pin(leftOUT, Pin.OUT, Pin.PULL_DOWN)
            btnLeftOUT.low()
        else:  # left+right are pressed
            btnRightOUT = Pin(rightOUT, Pin.IN)
            btnLeftOUT = Pin(leftOUT, Pin.IN)
    elif rightRead == False:
        btnLeftOUT = Pin(leftOUT, Pin.IN)
        btnRightOUT = Pin(rightOUT, Pin.OUT, Pin.PULL_DOWN)
        btnRightOUT.low()
    else:
        btnRightOUT = Pin(rightOUT, Pin.IN)
        btnLeftOUT = Pin(leftOUT, Pin.IN)

    if upRead == False:
        if downRead:
            btnDownOUT = Pin(downOUT, Pin.IN)
            btnUpOUT = Pin(upOUT, Pin.OUT, Pin.PULL_DOWN)
            btnUpOUT.low()
        else:
            btnUpOUT = Pin(upOUT, Pin.IN)
            btnDownOUT = Pin(downOUT, Pin.IN)
    elif downRead == False:
        btnDownOUT = Pin(downOUT, Pin.OUT, Pin.PULL_DOWN)
        btnDownOUT.low()
        btnUpOUT = Pin(upOUT, Pin.IN)
    else:
        btnUpOUT = Pin(upOUT, Pin.IN)
        btnDownOUT = Pin(downOUT, Pin.IN)

    if leftRead == False and rightRead == False and upRead == False and downRead == False:
        SOCD_speech_text = ' WTF'
    elif leftRead == False and rightRead == False and upRead and downRead:
        SOCD_speech_text = 'L+R:N'
    elif upRead == False and downRead == False and leftRead and rightRead:
        SOCD_speech_text = 'U+D:N'
    elif leftRead == False and rightRead == False and (upRead == False or downRead == False):
        SOCD_speech_text = 'L+R:N'
    elif upRead == False and downRead == False and (leftRead == False or rightRead == False):
        SOCD_speech_text = 'U+D:N'
    else:
        SOCD_speech_text = ' '


def useSOCD1():  # SOCD-U: left + right = neutral; down + up = up
    global btnUpOUT
    global btnDownOUT
    global btnLeftOUT
    global btnRightOUT
    global SOCD_speech_text
    global SOCD_last_value

    if leftRead == False:
        if rightRead:
            btnRightOUT = Pin(rightOUT, Pin.IN)
            btnLeftOUT = Pin(leftOUT, Pin.OUT, Pin.PULL_DOWN)
            btnLeftOUT.low()
        else:
            btnRightOUT = Pin(rightOUT, Pin.IN)
            btnLeftOUT = Pin(leftOUT, Pin.IN)
    elif rightRead == False:
        btnLeftOUT = Pin(leftOUT, Pin.IN)
        btnRightOUT = Pin(rightOUT, Pin.OUT, Pin.PULL_DOWN)
        btnRightOUT.low()
    else:
        btnRightOUT = Pin(rightOUT, Pin.IN)
        btnLeftOUT = Pin(leftOUT, Pin.IN)

    if upRead == False:
        btnDownOUT = Pin(downOUT, Pin.IN)
        btnUpOUT = Pin(upOUT, Pin.OUT, Pin.PULL_DOWN)
        btnUpOUT.low()
    elif downRead == False:
        btnDownOUT = Pin(downOUT, Pin.OUT, Pin.PULL_DOWN)
        btnDownOUT.low()
        btnUpOUT = Pin(upOUT, Pin.IN)
    else:
        btnUpOUT = Pin(upOUT, Pin.IN)
        btnDownOUT = Pin(downOUT, Pin.IN)

    if leftRead == False and rightRead == False and upRead == False and downRead == False:
        SOCD_speech_text = ' WTF'
    elif leftRead == False and rightRead == False and upRead and downRead:
        SOCD_speech_text = 'L+R:N'
    elif upRead == False and downRead == False and leftRead and rightRead:
        SOCD_speech_text = 'U+D:U'
    elif leftRead == False and rightRead == False and (upRead == False or downRead == False):
        SOCD_speech_text = 'L+R:N'
    elif upRead == False and downRead == False and (leftRead == False or rightRead == False):
        SOCD_speech_text = 'U+D:U'
    else:
        SOCD_speech_text = ' '


def useSOCD2():  # SOCD-L: left + right = last command wins; up + down = last command wins
    global upPreviousState
    global downPreviousState
    global leftPreviousState
    global rightPreviousState
    global currentMillis
    global upPreviousMillis
    global downPreviousMillis
    global leftPreviousMillis
    global rightPreviousMillis
    global SOCD_speech_text

    if upRead == False:
        if upPreviousState == False:
            upPreviousMillis = currentMillis
            upPreviousState = 1  # high true
        if downRead == False:
            if upPreviousMillis > downPreviousMillis:
                btnUpOUT = Pin(upOUT, Pin.OUT, Pin.PULL_DOWN)
                btnUpOUT.low()
            else:
                btnUpOUT = Pin(upOUT, Pin.IN)
        else:
            btnUpOUT = Pin(upOUT, Pin.OUT, Pin.PULL_DOWN)
            btnUpOUT.low()

    if upRead:
        upPreviousState = 0  # low false
        btnUpOUT = Pin(upOUT, Pin.IN)

    if downRead == False:
        if downPreviousState == False:
            downPreviousMillis = currentMillis
            downPreviousState = 1
        if upRead == False:
            if downPreviousMillis > upPreviousMillis:
                btnDownOUT = Pin(downOUT, Pin.OUT, Pin.PULL_DOWN)
                btnDownOUT.low()
            else:
                btnDownOUT = Pin(downOUT, Pin.IN)
        else:
            btnDownOUT = Pin(downOUT, Pin.OUT, Pin.PULL_DOWN)
            btnDownOUT.low()

    if downRead:
        downPreviousState = 0
        btnDownOUT = Pin(downOUT, Pin.IN)

    if leftRead == False:
        if leftPreviousState == False:
            leftPreviousMillis = currentMillis
            leftPreviousState = 1
        if rightRead == False:
            if leftPreviousMillis > rightPreviousMillis:
                btnLeftOUT = Pin(leftOUT, Pin.OUT, Pin.PULL_DOWN)
                btnLeftOUT.low()
            else:
                btnLeftOUT = Pin(leftOUT, Pin.IN)
        else:
            btnLeftOUT = Pin(leftOUT, Pin.OUT, Pin.PULL_DOWN)
            btnLeftOUT.low()

    if leftRead:
        leftPreviousState = 0
        btnLeftOUT = Pin(leftOUT, Pin.IN)

    if rightRead == False:
        if rightPreviousState == False:
            rightPreviousMillis = currentMillis
            rightPreviousState = 1
        if leftRead == False:
            if rightPreviousMillis > leftPreviousMillis:
                btnRightOUT = Pin(rightOUT, Pin.OUT, Pin.PULL_DOWN)
                btnRightOUT.low()
            else:
                btnRightOUT = Pin(rightOUT, Pin.IN)
        else:
            btnRightOUT = Pin(rightOUT, Pin.OUT, Pin.PULL_DOWN)
            btnRightOUT.low()

    if rightRead:
        rightPreviousState = 0
        btnRightOUT = Pin(rightOUT, Pin.IN)

    if leftRead == False and rightRead == False and upRead == False and downRead == False:
        SOCD_speech_text = ' WTF'
    elif leftRead == False and rightRead == False and upRead and downRead and leftPreviousMillis > rightPreviousMillis:
        SOCD_speech_text = 'L+R:L'
    elif leftRead == False and rightRead == False and upRead and downRead and leftPreviousMillis < rightPreviousMillis:
        SOCD_speech_text = 'L+R:R'
    elif upRead == False and downRead == False and leftRead and rightRead and downPreviousMillis > upPreviousMillis:
        SOCD_speech_text = 'U+D:U'
    elif upRead == False and downRead == False and leftRead and rightRead and downPreviousMillis < upPreviousMillis:
        SOCD_speech_text = 'U+D:D'
    elif leftRead == False and rightRead == False and (upRead == False or downRead == False) and leftPreviousMillis > rightPreviousMillis:
        SOCD_speech_text = 'L+R:L'
    elif leftRead == False and rightRead == False and (upRead == False or downRead == False) and leftPreviousMillis < rightPreviousMillis:
        SOCD_speech_text = 'L+R:R'
    elif upRead == False and downRead == False and (leftRead == False or rightRead == False) and downPreviousMillis > upPreviousMillis:
        SOCD_speech_text = 'U+D:U'
    elif upRead == False and downRead == False and (leftRead == False or rightRead == False) and downPreviousMillis < upPreviousMillis:
        SOCD_speech_text = 'U+D:D'
    else:
        SOCD_speech_text = ' '


def useSOCD3():  # SOCD-LU: left + right = last command wins; up + down = up
    global upPreviousState
    global downPreviousState
    global leftPreviousState
    global rightPreviousState
    global currentMillis
    global upPreviousMillis
    global downPreviousMillis
    global leftPreviousMillis
    global rightPreviousMillis
    global SOCD_speech_text

    if upRead == False:
        btnDownOUT = Pin(downOUT, Pin.IN)
        btnUpOUT = Pin(upOUT, Pin.OUT, Pin.PULL_DOWN)
        btnUpOUT.low()
    elif downRead == False:
        btnDownOUT = Pin(downOUT, Pin.OUT, Pin.PULL_DOWN)
        btnDownOUT.low()
        btnUpOUT = Pin(upOUT, Pin.IN)
    else:
        btnUpOUT = Pin(upOUT, Pin.IN)
        btnDownOUT = Pin(downOUT, Pin.IN)

    if leftRead == False:
        if leftPreviousState == False:
            leftPreviousMillis = currentMillis
            leftPreviousState = 1
        if rightRead == False:
            if leftPreviousMillis > rightPreviousMillis:
                btnLeftOUT = Pin(leftOUT, Pin.OUT, Pin.PULL_DOWN)
                btnLeftOUT.low()
            else:
                btnLeftOUT = Pin(leftOUT, Pin.IN)
        else:
            btnLeftOUT = Pin(leftOUT, Pin.OUT, Pin.PULL_DOWN)
            btnLeftOUT.low()

    if leftRead:
        leftPreviousState = 0
        btnLeftOUT = Pin(leftOUT, Pin.IN)

    if rightRead == False:
        if rightPreviousState == False:
            rightPreviousMillis = currentMillis
            rightPreviousState = 1
        if leftRead == False:
            if rightPreviousMillis > leftPreviousMillis:
                btnRightOUT = Pin(rightOUT, Pin.OUT, Pin.PULL_DOWN)
                btnRightOUT.low()
            else:
                btnRightOUT = Pin(rightOUT, Pin.IN)
        else:
            btnRightOUT = Pin(rightOUT, Pin.OUT, Pin.PULL_DOWN)
            btnRightOUT.low()

    if rightRead:
        rightPreviousState = 0
        btnRightOUT = Pin(rightOUT, Pin.IN)

    if leftRead == False and rightRead == False and upRead == False and downRead == False:
        SOCD_speech_text = ' WTF'
    elif leftRead == False and rightRead == False and upRead and downRead and leftPreviousMillis > rightPreviousMillis:
        SOCD_speech_text = 'L+R:L'
    elif leftRead == False and rightRead == False and upRead and downRead and leftPreviousMillis < rightPreviousMillis:
        SOCD_speech_text = 'L+R:R'
    elif upRead == False and downRead == False and leftRead and rightRead:
        SOCD_speech_text = 'U+D:U'
    elif leftRead == False and rightRead == False and (upRead == False or downRead == False) and leftPreviousMillis > rightPreviousMillis:
        SOCD_speech_text = 'L+R:L'
    elif leftRead == False and rightRead == False and (upRead == False or downRead == False) and leftPreviousMillis < rightPreviousMillis:
        SOCD_speech_text = 'L+R:R'
    elif upRead == False and downRead == False and (leftRead == False or rightRead == False):
        SOCD_speech_text = 'U+D:U'
    else:
        SOCD_speech_text = ' '


def useSOCD4():  # SOCD-R: raw output, but on dualsense: left + right = neutral; up + down = neutral
    global btnUpOUT
    global btnDownOUT
    global btnLeftOUT
    global btnRightOUT
    global SOCD_speech_text

    if upRead == False:  # if up is pressed
        # switch the output to actually output
        btnUpOUT = Pin(upOUT, Pin.OUT, Pin.PULL_DOWN)
        btnUpOUT.low()  # make sure it's low
    if upRead:  # if up is released
        # switch the output back to input for safety
        btnUpOUT = Pin(upOUT, Pin.IN)

    if downRead == False:
        btnDownOUT = Pin(downOUT, Pin.OUT, Pin.PULL_DOWN)
        btnDownOUT.low()
    if downRead:
        btnDownOUT = Pin(downOUT, Pin.IN)

    if leftRead == False:
        btnLeftOUT = Pin(leftOUT, Pin.OUT, Pin.PULL_DOWN)
        btnLeftOUT.low()
    if leftRead:
        btnLeftOUT = Pin(leftOUT, Pin.IN)

    if rightRead == False:
        btnRightOUT = Pin(rightOUT, Pin.OUT, Pin.PULL_DOWN)
        btnRightOUT.low()
    if rightRead:
        btnRightOUT = Pin(rightOUT, Pin.IN)

    if leftRead == False and rightRead == False and upRead == False and downRead == False:
        SOCD_speech_text = ' WTF'
    else:
        SOCD_speech_text = ' '

##### Graphics Helpers #####


def gCircle(x0, y0, r, color):
    graphics.circle(x0, y0, r, color)


def gLine(x0, y0, x1, y1, color):
    graphics.line(x0, y0, x1, y1, color)


def gTriangle(x0, y0, x1, y1, x2, y2, color):
    graphics.triangle(x0, y0, x1, y1, x2, y2, color)


def gRect(x0, y0, width, height, color):
    graphics.rect(x0, y0, width, height, color)


def gPixel(x0, y0, color):
    oled.pixel(x0, y0, color)


def gText(text, x0, y0):
    oled.text(text, x0, y0)


def gTextWithOffset(text, x0, y0, offsetX, offsetY=0):
    if offsetX == 8 and offsetY == 0:
        return oled.text(text, x0, y0)
    else:
        for i in range(len(text)):
            oled.text(text[i], i * offsetX + x0, i * offsetY + y0)

##### Graphics Dicts #####


def drawHitBoxCircles(circle, fill=1):
    circleDict = {
        'left': lambda: gCircle(15, 25, 7, fill),
        'down': lambda: gCircle(31, 25, 7, fill),
        'right': lambda: gCircle(47, 29, 7, fill),
        'up': lambda: gCircle(53, 55, 8, fill),
        'square': lambda: gCircle(65, 25, 7, fill),
        'x': lambda: gCircle(63, 41, 7, fill),
        'triangle': lambda: gCircle(81, 23, 7, fill),
        'circle': lambda: gCircle(79, 39, 7, fill),
        'r1': lambda: gCircle(97, 23, 7, fill),
        'r2': lambda: gCircle(95, 39, 7, fill),
        'l1': lambda: gCircle(113, 24, 7, fill),
        'l2': lambda: gCircle(111, 40, 7, fill),
        'home': lambda: gCircle(4, 3, 3, fill),
        'share': lambda: gCircle(18, 3, 3, fill),
        'options': lambda: gCircle(32, 3, 3, fill),
        'r3': lambda: gCircle(46, 3, 3, fill),
        'l3': lambda: gCircle(60, 3, 3, fill),
        'touch': lambda: gCircle(74, 3, 3, fill),
    }

    if circle == 'all':
        for key in circleDict:
            circleDict[key]()
        return

    return circleDict[circle]()


def drawHitBoxLabels(label):
    def drawTouchIcon():
        iconX = 71
        iconY = 8

        touchIcon = [
            (0, 1, 1, 1, 1, 1, 1, 0),
            (1, 1, 0, 0, 0, 0, 1, 1),
            (1, 0, 1, 1, 1, 1, 0, 1),
            (0, 1, 0, 0, 0, 0, 1, 0),
            (1, 1, 0, 1, 1, 0, 1, 1),
            (1, 0, 1, 0, 0, 1, 0, 1),
            (1, 0, 1, 0, 0, 1, 0, 1),
            (0, 1, 0, 1, 1, 0, 1, 0),
        ]

        for i in range(0, len(touchIcon)):
            for ii in range(0, len(touchIcon[i])):
                gPixel(iconX + ii, iconY + i, touchIcon[i][ii])

    labelDict = {
        'touch': lambda: drawTouchIcon(),
        'circle': lambda: gCircle(79, 39, 4, 1),
        'triangle': lambda: gTriangle(81, 19, 77, 26, 85, 26, 1),
        'square': lambda: gRect(62, 22, 7, 7, 1),
        'left': lambda: {
            gLine(11, 25, 19, 25, 1),
            gLine(11, 25, 14, 22, 1),
            gLine(11, 25, 14, 28, 1),
        },
        'down': lambda: {
            gLine(31, 21, 31, 29, 1),
            gLine(28, 26, 31, 29, 1),
            gLine(34, 26, 31, 29, 1),
        },
        'right': lambda: {
            gLine(48, 26, 51, 29, 1),
            gLine(43, 29, 51, 29, 1),
            gLine(48, 32, 51, 29, 1),
        },
        'up': lambda: {
            gLine(53, 50, 53, 60, 1),
            gLine(53, 50, 49, 54, 1),
            gLine(53, 50, 57, 54, 1),
        },
        'x': lambda: {
            gLine(60, 38, 66, 44, 1),
            gLine(60, 44, 66, 38, 1),
        },
        'r1': lambda: {
            gText('r', 91, 19),
            gText('1', 97, 19),
        },
        'r2': lambda: {
            gText('r', 89, 36),
            gText('2', 95, 36),
        },
        'l1': lambda: {
            gText('l', 106, 21),
            gText('1', 112, 21),
        },
        'l2': lambda: {
            gText('l', 104, 37),
            gText('2', 110, 37),
        },
        'home': lambda: {
            # outer walls
            gLine(1, 12, 1, 15, 1),
            gLine(7, 12, 7, 15, 1),
            # door
            gLine(1, 15, 3, 15, 1),
            gLine(7, 15, 5, 15, 1),
            gLine(3, 15, 3, 13, 1),
            gLine(5, 15, 5, 13, 1),
            gLine(5, 13, 3, 13, 1),
            # jut out from wall
            gLine(2, 12, 0, 12, 1),
            gLine(6, 12, 8, 12, 1),
            # lines to form roof
            gLine(0, 12, 4, 8, 1),
            gLine(8, 12, 4, 8, 1),
        },
        'share': lambda: {
            gCircle(20, 10, 1, 1),
            gCircle(16, 12, 1, 1),
            gCircle(20, 14, 1, 1),
            gLine(19, 10, 17, 12, 1),
            gLine(17, 12, 19, 14, 1),
        },
        'options': lambda: {
            gLine(29, 10, 35, 10, 1),
            gLine(29, 12, 35, 12, 1),
            gLine(29, 14, 35, 14, 1),
        },
        'r3': lambda: {
            gText('r', 40, 8),
            gText('3', 46, 9),
        },
        'l3': lambda: {
            gText('l', 55, 9),
            gText('3', 60, 9),
        },
    }

    if label == 'all':
        for key in labelDict:
            labelDict[key]()
        return

    return labelDict[label]()


def drawSOCDLabel(socdLabel):
    socdDict = {
        0: lambda: {
            gTextWithOffset('L+R:N', 93, 0, 7),
            gTextWithOffset('U+D:N', 93, 9, 7),
        },
        1: lambda: {
            gTextWithOffset('L+R:N', 93, 0, 7),
            gTextWithOffset('U+D:U', 93, 9, 7),
        },
        2: lambda: {
            gTextWithOffset('L+R:LC', 86, 0, 7),
            gTextWithOffset('U+D:LC', 86, 9, 7),
        },
        3: lambda: {
            gTextWithOffset('L+R:LC', 86, 0, 7),
            gTextWithOffset('U+D:U', 86, 9, 7),
        },
        4: lambda: {
            gTextWithOffset('L+R:RAW', 84, 0, 6),
            gTextWithOffset('U+D:RAW', 84, 9, 6),
        },
    }

    if socdLabel == 'all':
        for key in socdDict:
            socdDict[key]()
        return

    return socdDict[socdLabel]()


def drawArt(art):
    def drawModel(model, cursorX, cursorY):
        toDraw = []

        if model == 'pirate1':
            toDraw = [
                (0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0),
                (0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0),
                (0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0),
                (1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 1),
                (1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1),
                (0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0),
                (0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0),
                (0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0),
                (0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0),
                (0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0),
                (0, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0),
                (0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0),
                (0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0),
                (0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0),
                (0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0),
            ]
        elif model == 'pirate2':
            toDraw = [
                (0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
                (0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0),
                (0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0),
                (0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0),
                (1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 1),
                (1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1),
                (0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0),
                (0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0),
                (0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0),
                (0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0),
                (0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0),
                (0, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0),
                (0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0),
                (0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0),
                (0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0),
            ]
        elif model == 'octopus1':
            toDraw = [
                (0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0),
                (0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0),
                (0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0),
                (0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0),
                (0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0),
                (0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0),
                (1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0),
                (1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0),
                (0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0),
                (0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0),
                (0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0),
                (0, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0),
                (1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0),
                (1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0),
                (0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1),
                (1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1),
                (0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0),
            ]
        elif model == 'octopus2':
            toDraw = [
                (0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0),
                (0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0),
                (0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0),
                (0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0),
                (0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0),
                (0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0),
                (1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0),
                (0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0),
                (0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0),
                (0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0),
                (1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0),
                (0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0),
                (0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0),
                (1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0),
                (1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 1),
                (1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 1),
                (0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0),
            ]
        elif model == 'cat1':
            toDraw = [
                (0, 1, 0, 0, 0, 1, 0, 0, 0, 0),
                (0, 1, 1, 0, 1, 1, 0, 0, 0, 0),
                (0, 1, 1, 1, 1, 1, 0, 0, 1, 0),
                (0, 1, 0, 1, 0, 1, 0, 1, 0, 0),
                (0, 1, 1, 1, 1, 1, 0, 1, 0, 0),
                (0, 0, 1, 1, 1, 0, 0, 0, 1, 0),
                (0, 1, 1, 1, 1, 1, 0, 0, 1, 0),
                (0, 1, 1, 1, 1, 1, 0, 1, 0, 0),
                (1, 1, 1, 1, 1, 1, 1, 0, 0, 0),
            ]
        elif model == 'cat2':
            toDraw = [
                (0, 1, 0, 0, 0, 1, 0, 0, 0, 0),
                (0, 1, 1, 0, 1, 1, 0, 0, 0, 0),
                (0, 1, 1, 1, 1, 1, 0, 1, 0, 0),
                (0, 1, 0, 1, 0, 1, 0, 0, 1, 0),
                (0, 1, 1, 1, 1, 1, 0, 0, 1, 0),
                (0, 0, 1, 1, 1, 0, 0, 1, 0, 0),
                (0, 1, 1, 1, 1, 1, 0, 0, 1, 0),
                (0, 1, 1, 1, 1, 1, 0, 1, 0, 0),
                (1, 1, 1, 1, 1, 1, 1, 0, 0, 0),
            ]

        for i in range(0, len(toDraw)):
            for ii in range(0, len(toDraw[i])):
                gPixel(cursorX + ii, cursorY + i, toDraw[i][ii])

    artDict = {
        'pirate1': lambda: drawModel('pirate1', 0, 49),
        'pirate2': lambda: drawModel('pirate2', 0, 49),
        'octopus1': lambda: drawModel('octopus1', 29, 47),
        'octopus2': lambda: drawModel('octopus2', 29, 47),
        'cat1': lambda: drawModel('cat1', 16, 55),
        'cat2': lambda: drawModel('cat2', 16, 55),
    }

    return artDict[art]()


def chooseSOCD():
    global SOCD_mode

    if leftRead == False and rightRead and upRead and downRead:
        SOCD_mode = 0  # SOCD-N: left + right = neutral; up + down = neutral
        save_SOCD_state(SOCD_mode)

    # right held down - save to flash
    elif rightRead == False and leftRead and upRead and downRead:
        SOCD_mode = 1  # SOCD-U: left + right = neutral; down + up = up
        save_SOCD_state(SOCD_mode)

    # up held down - save to flash
    elif upRead == False and rightRead and leftRead and downRead:
        SOCD_mode = 2  # SOCD-L: left + right = last command wins; up + down = last command wins
        save_SOCD_state(SOCD_mode)

    # down held down - save to flash
    elif downRead == False and rightRead and upRead and leftRead:
        SOCD_mode = 3  # SOCD-LU: left + right = last command wins; up + down = up
        save_SOCD_state(SOCD_mode)

    # up and down held down - save to flash
    elif downRead == False and upRead == False and leftRead and rightRead:
        SOCD_mode = 4  # SOCD-R: raw output, but on dualsense: left + right = neutral; up + down = neutral
        save_SOCD_state(SOCD_mode)

    # reset, but it's turning off the dualsense, so i dont think this is quite right.
    # if something is wrong, you wouldn't really be able to run this anyway, because
    # the loop would probably have stopped
    # elif leftRead == False and downRead == False and rightRead == False and upRead:
    #     machine.reset()

    print('SOCD_mode: ', SOCD_mode)
    graphics.fill_rect(84, 0, 44, 16, 0)
    drawSOCDLabel(SOCD_mode)


# check for eixisting SOCD file / create SOCD file on controller start
try:
    with open('savedata.json', 'r') as f:
        data = json.load(f)
        SOCD_mode = data['SOCD']
except:
    SOCD_mode = 0
    print('SOCD mode variable not found. Starting with default SOCD.')


# pre-draw buttons and labels
print('SOCD_mode: ', SOCD_mode)
oled.fill(0)
drawHitBoxLabels('all')
drawHitBoxCircles('all')
drawSOCDLabel(SOCD_mode)
gText('SQUIBS', 80, 57)
drawArt('pirate1')
drawArt('octopus1')
drawArt('cat1')

while True:
    # need these to be updated before SOCD
    currentMillis = utime.ticks_ms()
    upRead = btnUpIN.value()
    downRead = btnDownIN.value()
    leftRead = btnLeftIN.value()
    rightRead = btnRightIN.value()

    if btnChooseSOCD.value() == False and SOCDChangeFlag:
        chooseSOCD()
        SOCDChangeFlag = False
        SOCDChangeFlag2 = True
    elif btnChooseSOCD.value() and SOCDChangeFlag2:
        SOCDChangeFlag = True
        SOCDChangeFlag2 = False

    # run proper SOCD
    if SOCD_mode == 0:
        useSOCD0()
    elif SOCD_mode == 1:
        useSOCD1()
    elif SOCD_mode == 2:
        useSOCD2()
    elif SOCD_mode == 3:
        useSOCD3()
    elif SOCD_mode == 4:
        useSOCD4()
    else:
        SOCD_mode = 0

    # render bubble & text once while held
    if SOCD_last_value != SOCD_speech_text:
        SOCD_last_value = SOCD_speech_text

        if SOCD_speech_text == ' ':
            drawSpeechText('clear')
            print('empty')
        else:
            drawSpeechText('clear')
            drawSpeechText()

    # read mode on outputs; kinda jank, but the only way I could figure out how to read the current mode on pins
    # reads I or O for current input/output mode
    upOUTRead = str(btnUpOUT).split('mode=')[1][0:1]
    downOUTRead = str(btnDownOUT).split('mode=')[1][0:1]
    leftOUTRead = str(btnLeftOUT).split('mode=')[1][0:1]
    rightOUTRead = str(btnRightOUT).split('mode=')[1][0:1]

    ##### Button Press Renders #####
    ### Land of flags and probably better ways to do this ###
    # up
    if upOUTRead == 'O' and upFlag:
        graphics.fill_circle(53, 55, 8, 1)
        upFlag = False
        upFlag2 = True
    elif upOUTRead == 'I' and upFlag2:
        graphics.fill_circle(53, 55, 8, 0)
        drawHitBoxCircles('up')
        drawHitBoxLabels('up')
        upFlag = True
        upFlag2 = False

    # down
    if downOUTRead == 'O' and downFlag:
        graphics.fill_circle(31, 25, 7, 1)
        downFlag = False
        downFlag2 = True
    elif downOUTRead == 'I' and downFlag2:
        graphics.fill_circle(31, 25, 7, 0)
        drawHitBoxCircles('down')
        drawHitBoxLabels('down')
        downFlag = True
        downFlag2 = False

    # left
    if leftOUTRead == 'O' and leftFlag:
        graphics.fill_circle(15, 25, 7, 1)
        leftFlag = False
        leftFlag2 = True
    elif leftOUTRead == 'I' and leftFlag2:
        graphics.fill_circle(15, 25, 7, 0)
        drawHitBoxCircles('left')
        drawHitBoxLabels('left')
        leftFlag = True
        leftFlag2 = False

    # right
    if rightOUTRead == 'O' and rightFlag:
        graphics.fill_circle(47, 29, 7, 1)
        rightFlag = False
        rightFlag2 = True
    elif rightOUTRead == 'I' and rightFlag2:
        graphics.fill_circle(47, 29, 7, 0)
        drawHitBoxCircles('right')
        drawHitBoxLabels('right')
        rightFlag = True
        rightFlag2 = False

    # square
    if btnSquare.value() == False and squareFlag:
        graphics.fill_circle(65, 25, 7, 1)
        squareFlag = False
        squareFlag2 = True
    elif btnSquare.value() and squareFlag2:
        graphics.fill_circle(65, 25, 7, 0)
        drawHitBoxCircles('square')
        drawHitBoxLabels('square')
        squareFlag = True
        squareFlag2 = False

    # triangle
    if btnTriangle.value() == False and triangleFlag:
        graphics.fill_circle(81, 23, 7, 1)
        triangleFlag = False
        triangleFlag2 = True
    elif btnTriangle.value() and triangleFlag2:
        graphics.fill_circle(81, 23, 7, 0)
        drawHitBoxCircles('triangle')
        drawHitBoxLabels('triangle')
        triangleFlag = True
        triangleFlag2 = False

    # x
    if btnX.value() == False and xFlag:
        graphics.fill_circle(63, 41, 7, 1)
        xFlag = False
        xFlag2 = True
    elif btnX.value() and xFlag2:
        graphics.fill_circle(63, 41, 7, 0)
        drawHitBoxCircles('x')
        drawHitBoxLabels('x')
        xFlag = True
        xFlag2 = False

    # circle
    if btnCircle.value() == False and circleFlag:
        graphics.fill_circle(79, 39, 7, 1)
        circleFlag = False
        circleFlag2 = True
    elif btnCircle.value() and circleFlag2:
        graphics.fill_circle(79, 39, 7, 0)
        drawHitBoxCircles('circle')
        drawHitBoxLabels('circle')
        circleFlag = True
        circleFlag2 = False

    # r1
    if btnR1.value() == False and r1Flag:
        graphics.fill_circle(97, 23, 7, 1)
        r1Flag = False
        r1Flag2 = True
    elif btnR1.value() and r1Flag2:
        graphics.fill_circle(97, 23, 7, 0)
        drawHitBoxCircles('r1')
        drawHitBoxLabels('r1')
        r1Flag = True
        r1Flag2 = False

    # l1
    if btnL1.value() == False and l1Flag:
        graphics.fill_circle(113, 24, 7, 1)
        l1Flag = False
        l1Flag2 = True
    elif btnL1.value() and l1Flag2:
        graphics.fill_circle(113, 24, 7, 0)
        drawHitBoxCircles('l1')
        drawHitBoxLabels('l1')
        l1Flag = True
        l1Flag2 = False

    # r2
    if btnR2.value() == False and r2Flag:
        graphics.fill_circle(95, 39, 7, 1)
        r2Flag = False
        r2Flag2 = True
    elif btnR2.value() and r2Flag2:
        graphics.fill_circle(95, 39, 7, 0)
        drawHitBoxCircles('r2')
        drawHitBoxLabels('r2')
        r2Flag = True
        r2Flag2 = False

    # l2
    if btnL2.value() == False and l2Flag:
        graphics.fill_circle(111, 40, 7, 1)
        l2Flag = False
        l2Flag2 = True
    elif btnL2.value() and l2Flag2:
        graphics.fill_circle(111, 40, 7, 0)
        drawHitBoxCircles('l2')
        drawHitBoxLabels('l2')
        l2Flag = True
        l2Flag2 = False

    # home
    if btnHome.value() == False and homeFlag:
        graphics.fill_circle(4, 3, 3, 1)
        homeFlag = False
        homeFlag2 = True
    elif btnHome.value() and homeFlag2:
        graphics.fill_circle(4, 3, 3, 0)
        drawHitBoxCircles('home')
        drawHitBoxLabels('home')
        homeFlag = True
        homeFlag2 = False

    # share
    if btnShare.value() == False and shareFlag:
        graphics.fill_circle(18, 3, 3, 1)
        shareFlag = False
        shareFlag2 = True
    elif btnShare.value() and shareFlag2:
        graphics.fill_circle(18, 3, 3, 0)
        drawHitBoxCircles('share')
        drawHitBoxLabels('share')
        shareFlag = True
        shareFlag2 = False

    # options
    if btnOptions.value() == False and optionsFlag:
        graphics.fill_circle(32, 3, 3, 1)
        optionsFlag = False
        optionsFlag2 = True
    elif btnOptions.value() and optionsFlag2:
        graphics.fill_circle(32, 3, 3, 0)
        drawHitBoxCircles('options')
        drawHitBoxLabels('options')
        optionsFlag = True
        optionsFlag2 = False

    # touch
    if btnTouch.value() == False and touchFlag:
        graphics.fill_circle(74, 3, 3, 1)
        touchFlag = False
        touchFlag2 = True
    elif btnTouch.value() and touchFlag2:
        graphics.fill_circle(74, 3, 3, 0)
        drawHitBoxCircles('touch')
        drawHitBoxLabels('touch')
        touchFlag = True
        touchFlag2 = False

    # r3 - R3 & L3 are active Highs, opposite of the rest
    if btnR3.value() and r3Flag:
        graphics.fill_circle(46, 3, 3, 1)
        r3Flag = False
        r3Flag2 = True
    elif btnR3.value() == False and r3Flag2:
        graphics.fill_circle(46, 3, 3, 0)
        drawHitBoxCircles('r3')
        drawHitBoxLabels('r3')
        r3Flag = True
        r3Flag2 = False

    # l3 - R3 & L3 are active Highs, opposite of the rest
    if btnL3.value() and l3Flag:
        graphics.fill_circle(60, 3, 3, 1)
        l3Flag = False
        l3Flag2 = True
    elif btnL3.value() == False and l3Flag2:
        graphics.fill_circle(60, 3, 3, 0)
        drawHitBoxCircles('l3')
        drawHitBoxLabels('l3')
        l3Flag = True
        l3Flag2 = False

    # art animation control
    art_animation_control -= 1
    if art_animation_control <= 0:
        art_animation_control = art_animation_speed
        art_animation_flag = True
    elif art_animation_control == round(art_animation_speed / 2):
        art_animation_flag = True

    if art_animation_control > round(art_animation_speed / 2) and art_animation_flag:
        drawArt('pirate1')
        drawArt('octopus1')
        drawArt('cat1')
        art_animation_flag = False
    elif art_animation_control < round(art_animation_speed / 2) and art_animation_flag:
        drawArt('pirate2')
        drawArt('octopus2')
        drawArt('cat2')
        art_animation_flag = False

    oled.show()
