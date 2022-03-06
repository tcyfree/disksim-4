## Disksim 是由卡内基梅隆大学开发的一款磁盘模拟工具。后来微软基于 disksim 开发了一个 SSD Extension 插件，使其可以模拟固态硬盘。

## 运行环境
Disksim 是基于`32位`的 linux 系统开发的，所以最好先确保你的系统是32位的。
或者使用Docker部署：
`
$ docker pull tcyfree/ubuntu14-i386-disksim-4.0-with-ssdmodel:v1
$ docker run -itd --name=disksim-with-ssdmodel tcyfree/ubuntu14-i386-disksim-4.0-with-ssdmodel:v1
$ docker exec -it disksim-with-ssdmodel bash  
`

## 快速使用
### 1. 源码下载：
```
$ git clone https://github.com/tcyfree/disksim-4.git disksim-with-ssdmodel

$ cd disksim-with-ssdmodel
```
### 2. 编译：
`
$ make
`
### 3. 测试：
#### （1）disksim-4.0
```
$ cd valid; ./runvalid
```

![disksim-4.0运行结果](https://raw.githubusercontent.com/tcyfree/disksim-4/disksim-with-ssdmodel/disksim-4.0.png)

#### （2）disksim-with-ssdmodel
```
$ chmod a+x ../ssdmodel/valid/runvalid

$ cd ../ssdmodel/valid; ./runvalid
```

![disksim-wit-ssdmodel运行结果](https://raw.githubusercontent.com/tcyfree/disksim-4/disksim-with-ssdmodel/disksim-with-ssdmodel.png)