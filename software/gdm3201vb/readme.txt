-- cn --
����Ҫ���ɹ����ļ�:
1. �޸� rtconfig.py :
- CROSS_TOOL='keil'           #ʹ�õĹ�����
- STM32_TYPE = 'STM32F10X_MD' #STM32��Ӧ���ͺ�.
2. �޸� rtconfig.h ������Ҫ�Ĺ���.
3. �Զ�����rt-threadĿ¼
- set RTT_ROOT=xxx
- scons --copy
4. ִ�� scons --target=mdk4 -s  ����MDK�����ļ�.

note: ��Ҫ��װ python,scons.

-- en --
