-- map input

i = lookup('/sys/servers/input')
i:beginmap()
    i:map('keyb0:f8.down',       'script:nebula.sys.servers.console:watch("*")')
    i:map('keyb0:f7.up',         'script:nebula.sys.servers.console:unwatch()')
    i:map('keyb0:esc.down',      'script:nebula.main:stop()')
    i:map('keyb0:f11.down',      'script:nebula.sys.servers.console:toggle()')
i:endmap()
