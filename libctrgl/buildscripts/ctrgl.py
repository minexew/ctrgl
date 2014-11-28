component('ctrgl', 'static-library')

if 'ctr' not in platform_tags:
	abort('This library only works on the CTR!')

header_directories('include')
source_all_in('source')
