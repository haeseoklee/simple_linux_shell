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
haeseok@haeseok-VirtualBox:/home/haeseok 🔥🔥🔥 pwd
/home/haeseok
```



### history command (요구사항 3번)

```shell
haeseok@haeseok-VirtualBox:/home/haeseok 🔥🔥🔥 cd ..
haeseok@haeseok-VirtualBox:/home 🔥🔥🔥 
```



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



### back