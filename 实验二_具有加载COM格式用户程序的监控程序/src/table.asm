function1:
	db 'function: up_lt block: 3'
	times 32-($-function1) db 0
function2:
	db 'function: up_rt block: 4'
	times 32-($-function2) db 0
function3:
	db 'function: dn_lt block: 5'
	times 32-($-function3) db 0
function4:
	db 'function: dn_rt block: 6'
	times 512-($-$$) db 0