# pvz-auto-click
This is a cheating script which automatically collects suns in Plants vs. Zombies.

See showcase video on [YouTube](https://youtu.be/rSpiloOeUQk).

<!-- I am too lazy to express my thoughts in another language for now. Maybe I will, maybe not. -->


## Statistics
Worked on this from July 1 to July 5, 2018.
Totally spent about 30 hours.

## I learned
- The basics in OpenCV
- RGB, HSV color space
- `cliclick` usages


## 通过这个项目，我学到了

- OpenCV 是什么，能干什么，和一些最最基础的用法。
- RGB、HSV 色域的特点、如何转换等。
- cliclick 用法。PS: 这个真的挺好的用的，Windows 上能不能用没试过。
- 复习了一下好久没碰的C++。

## How it works
- Use `screencapture` to capture a hard-coded region of the screen
- Convert color space to HSV and use threshhold and mask to capture possible locations of the "suns"
- Find the contours of the binary image, by restricting area, width/height ratio to eliminate false positives
- Use `cliclick` to simulate mouse movements and clicks

## 设计思路

- 使用`screencapture`来对指定屏幕区域截图，然后读取图像文件。
- 将图像转换成 HSV 色域，然后通过应用`threshhold`和`mask`来获取关于“太阳”的二值化图像。
- 从二值化图像中提取`contour`，通过限制区域面积、区域长宽比的方式来提取真“太阳”的位置。
- 使用`cliclick`操作鼠标。

## Things to improve
- Use a more flexiable way to capture the image
- Use `ObjectDetection` class for flexibility and readability


## 可以改进的地方

- 使用查找窗口名的方法，对窗口截图，而不是对于屏幕特定区域截图。这样可以适应不用的分辨率、不同的窗口位置等等。这一点暂时没有研究，凑活用着。
- 将识别图像的部分包装成`Object Detection`类。这样使用更便捷。现在这样写，维护很麻烦，用起来也很麻烦。
这个类的各种借口还没有仔细看，而且需要全部重新写这个东西，懒得弄，所以暂时搁置了。


## 总结

OpenCV 挺强大的，能干不少事情。本来是想做个全自动打植物大战僵尸的，结果发现坑开太大了。另外，本来是打算用 Python 写的，但当时 Python 还没理解
透彻，现在要是再写一遍，可能会用 Python 做，C++ 写起来还是不太友好。(／‵Д′)／~ ╧╧ 😵
