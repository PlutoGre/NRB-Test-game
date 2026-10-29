rm -f .\AndroidManifest.xml
cp -f .\AndroidManifest_900037.xml .\AndroidManifest.xml
call clearDIRs.bat
call antclean.bat
call step3_copy_res_src_libs_assets_so.bat
call step4_ant_build.bat
call winrar_hzip_conv.bat
call sign_apk.bat
call zipalign.bat 900037 0 

pause