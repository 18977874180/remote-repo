## 1、快速排序
#### 结构体函数
``` c
typedef _Range{
int start,end
}Range
Range new_Range(int s,int e){
	Range r;
	r.start=s;
	r.end=e;
	return r;
}
```
#### 交换函数
``` c
void swap(int *x;int *y){
	int temp=*x;
	*x=*y;
	*y=temp;
}
```
#### 快速排序函数
``` c
void quick_sort(int arr[],const int len){
    if (len<=0) return;
    Range r=[len];
    int p=0;
    r[p++]=new_Range(0,len-1);
    while(p){
        Range range=r[--p];
        if (range.start>=range.end) continue;
        int mid=arr[(range.start+range.end)/2]; //取中间为基数
        int left=range.start,right=range.end;
        do{
            while(arr[left]<mid) ++left;
            while(arr[right]>mid) --right;
            if (left <= right){
                swap(&arr[left],&arr[right]);
                left++;
                right--;
            }
        }while(left <= right);
        if(range.start < right) r[p++] = new_Range(range.sart,right);
        if(range.end > left) r[p++] = new_Range(left,range.end);
    }
}
```
## 2、冒泡排序
``` c
void swap(int a;int b){
	int temp;
	temp=a;
	a=b;
	b=temp;
}

void bubble_sort(int arr[],int len){
	int i,j,temp;
	for(i=0;i<len-1;i++)
		for(j=0;j<len-1-i;j++) // 每一趟最后一个就绪，每一趟可减少一次
			if(arr[j]>arr[j+1])
				swap(arr[j],arr[j+1]);
}
```
## 3、选择排序
``` c
void swap(int *a,int *b){
    
}
```
