### 源码路径下签名APK
**编译源码生成 `apksigner`工具**
使用基线里面默认的key签名apk
``` bash
~/t1102-dev/system/build/target/product/security

~/t1102-dev/system/out/target/product/qssi/obj/PACKAGING/otatools_intermediates/otatools/bin/apksigner sign \
 --key platform.pk8 \
 --cert platform.x509.pem \
 --out StressTest_signed.apk StressTest_2.0.8.0.apk
```
