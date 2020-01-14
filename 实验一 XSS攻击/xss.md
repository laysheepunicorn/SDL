# XSS攻击

## 实验背景

- 恶意攻击者往Web页面里插入恶意Script代码，当用户浏览该页面时，嵌入Web里面的Script代码会被执行，从而达到恶意攻击用户的目的。XSS攻击针对的是用户层面的攻击
- XSS攻击分类
  - 反射型XSS：非持久化，需要欺骗用户自己去点击链接才能触发XSS代码（服务器中没有这样的页面和内容），一般容易出现在搜索页面。反射型XSS大多数是用来盗取用户的Cookie信息。
  - 存储型XSS：存储型XSS，持久化，代码是存储在服务器中的，如在个人信息或发表文章等地方，插入代码，如果没有过滤或过滤不严，那么这些代码将储存到服务器中，用户访问该页面的时候触发代码执行。这种XSS比较危险，容易造成蠕虫，盗窃cookie。
  - DOM型XSS：不经过后端，DOM-XSS漏洞是基于文档对象模型(Document Objeet Model,DOM)的一种漏洞，DOM-XSS是通过url传入参数去控制触发的，其实也属于反射型XSS。
- XSS的攻击载荷
  - \<script\>标签
  - svg标签：
  - \<img\>标签：
  - \<body\>标签：
  - video标签
  - style标签

## 实验环境配置

- apache配置
  - 改conv/httpd.conv的Define SRVROOT路径为存放地址 Listen 80改为Listen 88，#ServerName www.example.com:80去掉#并改为ServerName www.example.com:88
  - 以管理员身份打开命令提示符，进入路径httpd-2.4.33-win64-VC15\Apache24\bin中，输入httpd.exe -k install，再使用net start Apache2.4打开apache，在浏览器中输入<http://localhost:88/>，显示It's works.
- php配置
  - 设置php的扩展路径
    - 查找 ;extension_dir = "./" ，在后面添加  
    extension_dir = "ext"  
    extension=php_mbstring.dll  
    extension=php_mysql.dll  
    extension=php_mysqli.dll
  - 查找 ;date.timezone =设置为date.timezone = Asia/Shanghai
- Apache与php7关联
  - 打开httpd.conf查找#LoadModule xml2enc_module modules/mod_xml2enc.so的部分，并在最后追加如下代码：  
    ```
    LoadModule php7_module "php7apache2_4.dll的绝对路径"
    ```
  - 查找#AddEncoding x-gzip .gz .tgz的部分，并在最后追加如下现行代码：
    ```
    AddType application/x-httpd-php .php
    AddType application/x-httpd-php-source .phps
    ```
  - 指定php.ini配置文件路径  
  在httpd.conf文件里指定PHP配置文件php.ini的目录，这里定位到httpd.conf文件的未尾，在文本最后面一行添加代码：PHPIniDir "D:\Grade-three\SDL\enviroment\php-7.4.1-Win32-vc15-x64"
  - 配置DirectoryIndex  
  查找“DirectoryIndex”的部分，在DirectoryIndex的后面追加一个“index.php  index.htm”
  - 将下面两行代码的注释（#号）去掉
    ```
    #LoadModule rewrite_module modules/mod_rewrite.so
    #LoadModule vhost_alias_module modules/mod_vhost_alias.so
    ```

## 实验过程

- 我们的实验主要模拟反射型xss攻击过程
- 编写index1.html前端页面，包含内容框和提交按键，提交内容之后后端的php代码会反馈给浏览器，这里的简单php代码使浏览器反馈刚才输入的内容
  ![ ](image\反射型xss页面.JPG)
  ![ ](image\反射型xss页面php反馈.JPG)
- 往输入框中输入```<script>alert('hack')</script>```
  ![ ](image\反射型xss攻击.JPG)
- 可以观察到alert框的弹出
  ![ ](image\反射型xss攻击结果.JPG)