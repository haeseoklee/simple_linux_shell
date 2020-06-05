# MY SHELL 🔥🔥🔥

MyShell 은 한양대학교 ERICA 소프트웨어융합대학 소프트웨어학부 시스템 프로그래밍 수업의 학기 프로젝트 제출용으로 제작되었습니다.

Linux 와 Mac OS 을 지원합니다.

MyShell의 모든 저작권은 소프트웨어학부 컴퓨터전공 이해석에게 있습니다.



## 빌드 및 실행방법



### 빌드

```  shell
$ make
```



### 실행

```shell
$ ./myshell.out
```



##  사용방법



### foreground and background execution (요구사항 1번)



``` shell
haeseok@haeseok-VirtualBox:/home/haeseok 🔥🔥🔥 ls
Desktop    Downloads	     linux-5.5.13.tar.xz  myhomework  mypstree	Public	   Videos     Documents  examples.desktop  Music		  mymodule    Pictures	Templates
```



```
haeseok@haeseok-VirtualBox:/home/haeseok 🔥🔥🔥 ls & pwd
[1] 6661
/home/haeseok
haeseok@haeseok-VirtualBox:/home/haeseok 🔥🔥🔥 abc	   Downloads		Music	    mypstree  Templates
Desktop    examples.desktop	myhomework  Pictures  Videos
Documents  linux-5.5.13.tar.xz	mymodule    Public
```



``` shell
haeseok@haeseok-VirtualBox:/home/haeseok 🔥🔥🔥 who & ls & 
[1] 6667
[2] 6668
haeseok@haeseok-VirtualBox:/home/haeseok 🔥🔥🔥 abc	   Downloads		Music	    mypstree  Templates
Desktop    examples.desktop	myhomework  Pictures  Videos
Documents  linux-5.5.13.tar.xz	mymodule    Public
haeseok  :0           2020-06-01 20:30 (:0)
```



### multiple commands (요구사항 2번)

``` shell
haeseok@haeseok-VirtualBox:/home/haeseok 🔥🔥🔥 pwd ; date ; whoami          
/home/haeseok
2020. 06. 05. (금) 12:10:15 KST
haeseok
```



```
haeseok@haeseok-VirtualBox:/home/haeseok 🔥🔥🔥 (sleep 10; echo done)
done
haeseok@haeseok-VirtualBox:/home/haeseok 🔥🔥🔥 (ls -al | grep info) > info.txt
haeseok@haeseok-VirtualBox:/home/haeseok 🔥🔥🔥 cat info.txt
-rwxr-xr-x  1 haeseok haeseok         0  6월  5 12:29 info.txt
-rw-------  1 haeseok haeseok     13701  5월 30 14:14 .viminfo
haeseok@haeseok-VirtualBox:/home/haeseok 🔥🔥🔥 (sleep 3 ; echo test) ; (pwd ; date)
test
/home/haeseok
2020. 06. 05. (금) 12:31:04 KST
```



### history command (요구사항 3번)

``` shell
haeseok@haeseok-VirtualBox:/home/haeseok 🔥🔥🔥 history
 1  ls
 2  ls -al
 3  history
 4  history > abc
 5  cat abc
 6  pwd
 7  who
 8  whoami
 9  cd ..
 10  sudo cd Desktop
 11  cd Desktop
 12  cd ..
 13  cd Desktop
 14  history
```



```shell
 17  (sleep 1; echo done)
 18  sleep 1; echo done
 19  (ls)
 20  ls & pwd
 21  pwd & whoami &
 22  who & ls &
 23  pwd ; date ; whoami
 24  history
haeseok@haeseok-VirtualBox:/home/haeseok 🔥🔥🔥 !23
pwd ; date ; whoami
/home/haeseok
2020. 06. 05. (금) 12:11:32 KST
haeseok
```



```shell
 8  whoami
 9  cd ..
 10  sudo cd Desktop
 11  cd Desktop
 12  cd ..
 13  cd Desktop
 14  history
 15  (sleep 10; echo done)&
 16  (sleep 1; echo done)&
 17  (sleep 1; echo done)
 18  sleep 1; echo done
 19  (ls)
 20  ls & pwd
 21  pwd & whoami &
 22  who & ls &
 23  pwd ; date ; whoami
 24  history
 25  pwd ; date ; whoami
 26  date
 27  history
 28  ls
 29  history
haeseok@haeseok-VirtualBox:/home/haeseok 🔥🔥🔥 !26 ; !28 ; !8
date ; ls ; whoami
2020. 06. 05. (금) 12:13:15 KST
abc	   Downloads		Music	    mypstree  Templates
Desktop    examples.desktop	myhomework  Pictures  Videos
Documents  linux-5.5.13.tar.xz	mymodule    Public
haeseok
```



```shell
 62  (ls -al | grep info) > info.txt
 63  cat info.txt
 64  (sleep 3 ; test) ; (pwd ; date)
 65  (sleep 3 ; echo test) ; (pwd ; date)
 66  (ls -al | grep info)
 67  history
haeseok@haeseok-VirtualBox:/home/haeseok 🔥🔥🔥 !66 > test.txt
(ls -al | grep info) > test.txt
haeseok@haeseok-VirtualBox:/home/haeseok 🔥🔥🔥 cat test.txt
-rwxr-xr-x  1 haeseok haeseok       128  6월  5 12:29 info.txt
-rw-------  1 haeseok haeseok     13701  5월 30 14:14 .viminfo
```



### shell redirection (>, 2>, >>, >|, <)   (요구사항 4번)

```shell
haeseok@haeseok-VirtualBox:/home/haeseok 🔥🔥🔥 ls > list1.txt
haeseok@haeseok-VirtualBox:/home/haeseok 🔥🔥🔥 cat list1.txt
abc
Desktop
Documents
Downloads
examples.desktop
linux-5.5.13.tar.xz
list1.txt
Music
myhomework
mymodule
mypstree
Pictures
Public
Templates
Videos
```



```shell
haeseok@haeseok-VirtualBox:/home/haeseok 🔥🔥🔥 cat none.txt 2> list2.txt
haeseok@haeseok-VirtualBox:/home/haeseok 🔥🔥🔥 cat list2.txt
cat: none.txt: No such file or directory
```



```shell
haeseok@haeseok-VirtualBox:/home/haeseok 🔥🔥🔥 who >> list2.txt    
haeseok@haeseok-VirtualBox:/home/haeseok 🔥🔥🔥 cat list2.txt
cat: none.txt: No such file or directory
haeseok  :0           2020-06-01 20:30 (:0)
```



```shell
haeseok@haeseok-VirtualBox:/home/haeseok 🔥🔥🔥 cat < list2.txt
cat: none.txt: No such file or directory
haeseok  :0           2020-06-01 20:30 (:0)
```



```shell
haeseok@haeseok-VirtualBox:/home/haeseok 🔥🔥🔥 ls > abc.txt    
haeseok@haeseok-VirtualBox:/home/haeseok 🔥🔥🔥 set -o noclobber 
haeseok@haeseok-VirtualBox:/home/haeseok 🔥🔥🔥 ls > abc.txt
myshell: abc.txt: cannot overwrite existing file
haeseok@haeseok-VirtualBox:/home/haeseok 🔥🔥🔥 pwd >| abc.txt
haeseok@haeseok-VirtualBox:/home/haeseok 🔥🔥🔥 cat abc.txt
/home/haeseok
haeseok@haeseok-VirtualBox:/home/haeseok 🔥🔥🔥 set +o noclobber
haeseok@haeseok-VirtualBox:/home/haeseok 🔥🔥🔥 ls > abc.txt
haeseok@haeseok-VirtualBox:/home/haeseok 🔥🔥🔥 cat abc.txt
abc
abc.txt
Desktop
Documents
Downloads
examples.desktop
linux-5.5.13.tar.xz
list1.txt
list2.txt
Music
myhomework
mymodule
mypstree
Pictures
Public
Templates
Videos
```



### shell pipe (요구사항 5번)

```shell
haeseok@haeseok-VirtualBox:/home/haeseok 🔥🔥🔥 ls -al | grep vim
drwxr-xr-x  2 haeseok haeseok      4096  4월 17 13:35 .vim
-rw-------  1 haeseok haeseok     13701  5월 30 14:14 .viminfo
-rw-r--r--  1 haeseok haeseok        61  4월 24 12:20 .vimrc
```



### multiple pipe (요구사항 6번)

```shell
haeseok@haeseok-VirtualBox:/home/haeseok 🔥🔥🔥 ls -al | grep vim | grep rc
-rw-r--r--  1 haeseok haeseok        61  4월 24 12:20 .vimrc
```



```shell
haeseok@haeseok-VirtualBox:/home/haeseok 🔥🔥🔥 ps -ef | grep shell | grep my
haeseok   6765  6618  0 12:29 pts/0    00:00:00 ./myshell.out
```



### cd command (요구사항 7번)



```shell
haeseok@haeseok-VirtualBox:/home/haeseok 🔥🔥🔥 cd ..
haeseok@haeseok-VirtualBox:/home 🔥🔥🔥 
```