# 介绍
正在写的图形引擎，除了可以以窗口化运行外,还可以以动态壁纸方式运行，此外还可以开启动画渲染模式。

# 动画渲染模式需要注意的事项
1.确保使用的是NVIDIA显卡。

2.默认状态是以60帧渲染60秒，你可以通过Graphics::setRecordConfig这个函数来修改。

PS：INTEL的QSV还有AMD的AMF应该有相关的API来驱动显卡编码。