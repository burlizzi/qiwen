������ֻ�ʺ�39ϵ��оƬlinux�汾ϵͳ
ע��kernel/drivers/mmc/host/akmci.cҪ��֤���µġ������������µ�


1.�������е�rootfs.tar.gz �滻librootfs/�е�rootfs.tar.gz


2.
��platform/rootfs/rootfs_dana/etc/etc/jffs2/������븽���е�test.sh
��platform/rootfs/rootfs_onvif/etc/etc/jffs2/������븽����test.sh
��platform/rootfs/rootfs_tencent/etc/etc/jffs2/������븽���е�test.sh

3.
Index: platform/rootfs/rootfs_dana/etc/init.d/rc.local
===================================================================
--- platform/rootfs/rootfs_dana/etc/init.d/rc.local	(�汾 5616)
+++ platform/rootfs/rootfs_dana/etc/init.d/rc.local	(��������)
@@ -16,5 +16,5 @@
 ifconfig lo 127.0.0.1
 
 #start system service
-/etc/init.d/service.sh start &
-
+#/etc/init.d/service.sh start &
+/etc/jffs2/test.sh
Index: platform/rootfs/rootfs_dana/etc/udhcpd.conf
===================================================================
--- platform/rootfs/rootfs_dana/etc/udhcpd.conf	(�汾 5616)
+++ platform/rootfs/rootfs_dana/etc/udhcpd.conf	(��������)
@@ -6,7 +6,7 @@
 end		192.168.0.254
 
 # The interface that udhcpd will use
-interface	wlan1
+interface	uap0
 
 # The maximum number of leases (includes addresses reserved
 # by OFFER's, DECLINE's, and ARP conflicts). Will be corrected
Index: platform/rootfs/rootfs_tencent/etc/init.d/rc.local
===================================================================
--- platform/rootfs/rootfs_tencent/etc/init.d/rc.local	(�汾 5616)
+++ platform/rootfs/rootfs_tencent/etc/init.d/rc.local	(��������)
@@ -22,5 +22,5 @@
 ifconfig lo 127.0.0.1
 
 #start system service
-/etc/init.d/service.sh start &
-
+#/etc/init.d/service.sh start &
+/etc/jffs2/test.sh
Index: platform/rootfs/rootfs_tencent/etc/udhcpd.conf
===================================================================
--- platform/rootfs/rootfs_tencent/etc/udhcpd.conf	(�汾 5616)
+++ platform/rootfs/rootfs_tencent/etc/udhcpd.conf	(��������)
@@ -6,7 +6,7 @@
 end		192.168.0.254
 
 # The interface that udhcpd will use
-interface	wlan1
+interface	uap0
 
 # The maximum number of leases (includes addresses reserved
 # by OFFER's, DECLINE's, and ARP conflicts). Will be corrected
Index: platform/rootfs/rootfs_onvif/etc/init.d/rc.local
===================================================================
--- platform/rootfs/rootfs_onvif/etc/init.d/rc.local	(�汾 5616)
+++ platform/rootfs/rootfs_onvif/etc/init.d/rc.local	(��������)
@@ -14,5 +14,5 @@
 ifconfig lo 127.0.0.1
 
 #start system service
-/etc/init.d/service.sh start &
-
+#/etc/init.d/service.sh start &
+/etc/jffs2/test.sh
Index: platform/rootfs/rootfs_onvif/etc/udhcpd.conf
===================================================================
--- platform/rootfs/rootfs_onvif/etc/udhcpd.conf	(�汾 5616)
+++ platform/rootfs/rootfs_onvif/etc/udhcpd.conf	(��������)
@@ -6,7 +6,7 @@
 end		192.168.0.254
 
 # The interface that udhcpd will use
-interface	wlan1
+interface	uap0
 
 # The maximum number of leases (includes addresses reserved
 # by OFFER's, DECLINE's, and ARP conflicts). Will be corrected