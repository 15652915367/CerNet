# CerNet

app.Android 为安卓端应用层代码，Android版本19.0以上即可
部署步骤：
  1、Android Studio打开源码，连接手机
  2、点击运行
  或
  直接安装 AntVanet.apk

app.Web 为前端云平台展示代码
  
  部署环境要求 PHP 5+ + Mysql + Tomcat/Apache

NAT.config 为NAT配置
GeoNetworking 为基于地理位置路由的网络协议代码
  
  部署环境 Linux内核3.14.12及以上，src文件夹内使用make指令，主机名配置为RSUx或者OBUx，配置文件夹内3个脚本，启动指令为./startOBU-sample.sh

11pDiff 为驱动层修改
  修改linux内核驱动：mac80211、cfg80211、ath、和ath5k驱动
