#include <linux/input.h>
#include <linux/uinput.h>
#include <sys/time.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <poll.h>

#define die(str, args...) do { perror(str); exit(EXIT_FAILURE); } while(0)

// settings!
const int left_speed = CHANGE_ME;
const int right_speed = CHANGE_ME;
const int override_speed = CHANGE_ME;

const int override_key_code = KEY_E;        // press and hold to use the override speed
const int toggle_key_code = KEY_GRAVE;      // press to toggle turnbinds on/off (`)

void emit(int fd, int type, int code, int val) {
    struct input_event ie;

    ie.type = type;
    ie.code = code;
    ie.value = val;
    gettimeofday(&ie.time, NULL);

    if(write(fd, &ie, sizeof(ie)) < 0) perror("write uinput");
}

int read_mouse_buttons(int fd, bool *left_down, bool *right_down) {
    struct input_event ev;

    while(read(fd,&ev,sizeof(ev)) == sizeof(ev)) {
        if(ev.type == EV_KEY) {
            if(ev.code == BTN_LEFT) *left_down = (ev.value != 0);
            else if(ev.code == BTN_RIGHT) *right_down = (ev.value != 0);
        }
    }
    
    return 0;
}

int main(void) {
    int ufd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if(ufd < 0) die("open uinput");

    ioctl(ufd, UI_SET_EVBIT, EV_KEY);
    ioctl(ufd, UI_SET_KEYBIT, BTN_LEFT);
    ioctl(ufd, UI_SET_KEYBIT, BTN_RIGHT);
    ioctl(ufd, UI_SET_EVBIT, EV_REL);
    ioctl(ufd, UI_SET_RELBIT, REL_X);
    ioctl(ufd, UI_SET_RELBIT, REL_Y);
    ioctl(ufd, UI_SET_EVBIT, EV_SYN);

    struct uinput_setup usetup = {0};
    snprintf(usetup.name, UINPUT_MAX_NAME_SIZE, "virtual-mouse");
    usetup.id.bustype = BUS_USB;
    usetup.id.vendor  = 0x1234;
    usetup.id.product = 0x5678;
    usetup.id.version = 1;

    ioctl(ufd, UI_DEV_SETUP, &usetup);
    ioctl(ufd, UI_DEV_CREATE);
    sleep(1);

    // adjust these to your actual input devices (dev/input/eventX, X=YOUR DEVICE)
    int kb_fd = open("/dev/input/eventX", O_RDONLY | O_NONBLOCK);       // Keyboard
    int mouse_fd = open("/dev/input/eventX", O_RDONLY | O_NONBLOCK);    // Mouse

    if(kb_fd < 0 || mouse_fd < 0) die("open input devices");

    struct pollfd fds[2] = {
        { .fd = kb_fd, .events = POLLIN },
        { .fd = mouse_fd, .events = POLLIN }
    };

    bool left_down = false, right_down = false;
    bool override_down = false;
    bool turnbinds_enabled = true;

    printf("Turnbinds enabled\n");

    while(1) {
        if(poll(fds, 2, 10) < 0) die("poll");

        if(fds[0].revents & POLLIN) {
            struct input_event ev;
            
            while(read(kb_fd, &ev, sizeof(ev)) == sizeof(ev)) {
                if(ev.type == EV_KEY) {
                    if(ev.code == override_key_code) override_down = (ev.value != 0);
                    else if(ev.code == toggle_key_code && ev.value == 1) {
                        turnbinds_enabled = !turnbinds_enabled;
                        system("clear");
                        printf("Turnbinds %s\n", turnbinds_enabled ? "ENABLED" : "DISABLED");
                    }
                }
            }
        }
        
        read_mouse_buttons(mouse_fd, &left_down, &right_down);

        if(turnbinds_enabled) {
            int ls = override_down ? override_speed : left_speed;
            int rs = override_down ? override_speed : right_speed;

            if(left_down && !right_down) {
                emit(ufd, EV_REL, REL_X, -ls);
                emit(ufd, EV_SYN, SYN_REPORT, 0);
            } else if(right_down && !left_down) {
                emit(ufd, EV_REL, REL_X, rs);
                emit(ufd, EV_SYN, SYN_REPORT, 0);
            }
        }

        usleep(6060); // for a 165Hz monitor
        // usleep() changes the smoothness and sensitvitiy (requires look_left, look_right & override_speed to be changed as well!!)
        // the number you put here is the number you got from "sudo libinput debug-events --device /dev/input/eventX" after "POINTER_MOTION"
        // in my case: 6446 / 165 ≈ 39.06666667
    }

    ioctl(ufd, UI_DEV_DESTROY);
    close(ufd);
    return 0;
}
