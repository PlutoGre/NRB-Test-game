rm -f .\AndroidManifest.xml
cp -f .\AndroidManifest_wyx_100010041000.xml .\AndroidManifest.xml
call clearDIRs.bat
call antclean.bat
call step3_copy_res_src_libs_assets_so.bat
call step4_ant_build.bat
call winrar_hzip_conv.bat
call sign_apk.bat
call zipalign.bat 100010041000

pause