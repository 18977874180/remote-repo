## 回调函数（Callback Function）
### 1. 什么是回调函数？
回调函数（Callback Function） 是一种通过 函数指针 调用的函数。把函数的地址作为参数传递给另一个函数，这个函数在合适的时机再调用传进去的函数。
常用于：事件处理、库函数调用用户自定义逻辑、驱动和应用之间的接口等。
### 2. 基本语法

``` c
// 定义一个函数指针类型，指向返回值为int、参数为int的函数
typedef int (*callback_t)(int);

// 普通函数，符合callback_t的签名
int my_callback(int x) {
    printf("my_callback called with %d\n", x);
    return x * 2;
}

// 一个函数，接收回调函数作为参数
void invoke_callback(callback_t cb, int value) {
    int result = cb(value);  // 调用回调函数
    printf("Callback result: %d\n", result);
}

int main() {
    invoke_callback(my_callback, 10); // 把函数作为参数传进去
    return 0;
}
```

## 3. 常见使用场景

### （1）排序函数 `qsort`

C 标准库 `qsort` 就是一个典型的回调函数用法：

```c
#include <stdio.h>
#include <stdlib.h>

int compare_int(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

int main() {
    int arr[] = {42, 5, 17, 99, 8};
    int n = sizeof(arr)/sizeof(arr[0]);

    qsort(arr, n, sizeof(int), compare_int); // compare_int是回调函数

    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    return 0;
}
```

### （2）驱动与应用交互

在驱动层或库代码中，常常会定义“事件通知”的接口，用户把自己的函数注册进去：

``` c
#include <stdio.h>

// 定义回调函数类型
typedef void (*event_cb_t)(const char *msg);

// 框架内部保存一个回调
event_cb_t g_event_handler = NULL;

// 注册回调
void register_event_handler(event_cb_t cb) {
    g_event_handler = cb;
}

// 模拟触发事件
void trigger_event() {
    if (g_event_handler) {
        g_event_handler("Event happened!");
    }
}

// 用户自己实现的处理逻辑
void my_event_handler(const char *msg) {
    printf("User received: %s\n", msg);
}

int main() {
    register_event_handler(my_event_handler); // 注册
    trigger_event(); // 驱动或框架触发事件
    return 0;
}
```

⚠️ **缺点**

- 过度使用会降低代码可读性（尤其是多层回调）。
- 需要小心函数指针的类型和调用约定，错误可能导致崩溃。
