rm -f .\AndroidManifest.xml
cp -f .\AndroidManifest_wyx_100010041000.xml .\AndroidManifest.xml
call clearDIRs.bat
call antclean.bat
call step3_copy_res_src_libs_assets_so.bat
call step4_ant_build.bat
call winrar_hzip_conv.bat
call sign_apk.bat
call zipalign.bat 100010041000

rm -f .\AndroidManifest.xml
cp -f .\AndroidManifest_wyx_100010051000.xml .\AndroidManifest.xml
call clearDIRs.bat
call antclean.bat
call step3_copy_res_src_libs_assets_so.bat
call step4_ant_build.bat
call winrar_hzip_conv.bat
call sign_apk.bat
call zipalign.bat 100010051000

rm -f .\AndroidManifest.xml
cp -f .\AndroidManifest_wyx_100610061000.xml .\AndroidManifest.xml
call clearDIRs.bat
call antclean.bat
call step3_copy_res_src_libs_assets_so.bat
call step4_ant_build.bat
call winrar_hzip_conv.bat
call sign_apk.bat
call zipalign.bat 100610061000

rm -f .\AndroidManifest.xml
cp -f .\AndroidManifest_wyx_101010001000.xml .\AndroidManifest.xml
call clearDIRs.bat
call antclean.bat
call step3_copy_res_src_libs_assets_so.bat
call step4_ant_build.bat
call winrar_hzip_conv.bat
call sign_apk.bat
call zipalign.bat 101010001000

rm -f .\AndroidManifest.xml
cp -f .\AndroidManifest_wyx_101310001000.xml .\AndroidManifest.xml
call clearDIRs.bat
call antclean.bat
call step3_copy_res_src_libs_assets_so.bat
call step4_ant_build.bat
call winrar_hzip_conv.bat
call sign_apk.bat
call zipalign.bat 101310001000

rm -f .\AndroidManifest.xml
cp -f .\AndroidManifest_wyx_101510001000.xml .\AndroidManifest.xml
call clearDIRs.bat
call antclean.bat
call step3_copy_res_src_libs_assets_so.bat
call step4_ant_build.bat
call winrar_hzip_conv.bat
call sign_apk.bat
call zipalign.bat 101510001000
