#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <netinet/ip_icmp.h>
#include <netinet/if_ether.h>
#include <linux/if.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <pthread.h>

int EndFlag=0; // 終了処理に進むかを管理する
int DeviceSoc; // PF_PACKETのディスクリプタ
PARAM Param; // 設定を保持する構造体

// 送受信を担うスレッド
void *MyEthThread(void *arg) {
    int nready;
    /*
    ファイルディスクリプタの配列.
    struct pollfd {
        int fd;
        short events; 監視したいイベントを設定
        short revents; 実際に起こったイベントが設定される
    }
    */
    struct pollfd targets[1]; 
    u_int8_t buf[2048];
    int len;

    targets[0].fd = DeviceSoc;
    targets[0].events = POLIIN|POLLERR; // POLLIN: 読みだし可能, POLLERR: エラー

    while (EndFlag == 0) {
        // 少なくとも1つのファイルディスクリプタがI/Oを実行可能になるまで（イベントが設定されるまで）待機
        // poll(struct pollfd *fds, fdsの個数, timeout) https://manpages.ubuntu.com/manpages/bionic/ja/man2/poll.2.html
        // revents要素を持つ構造体の数を返す. 0はタイムアウト. エラーは-1でerrnoも設定される

        switch(nready=poll(targets, 1, 1000)) {
            case -1:
                if(errno != EINTR) {
                    perror("poll");
                }
                break;
            case 0:
                break;
            default:
                if(targets[0].revents & (POLLIN | POLERR)) {
                    if ((len=read(DeviceSoc, buf, sizeof(buf)))<=0) {
                        perror("read")
                    } else {
                        EtherRecv(DeviceSoc, buf, len);
                    }
                }
                break;

        }
    }
    return (NULL)
}