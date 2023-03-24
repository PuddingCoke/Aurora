# 介绍
正在写的图形引擎，可以以动态壁纸方式运行，还可以开启动画渲染模式。

# 动画渲染模式需要注意的事项
1.确保使用的是Nvidia显卡。

2.安装并配置好FFMPEG。

3.默认状态是以60帧渲染60秒，你可以通过Graphics::setRecordConfig这个函数来修改。

PS：INTEL的QSV还有AMD的AMF应该有相关的API来驱动显卡编码。