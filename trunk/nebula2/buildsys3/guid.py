#--------------------------------------------------------------------------
# GUID Generator
#
# (c) 2005 Vadim Macagon
#
# Contents are licensed under the Nebula license.
#--------------------------------------------------------------------------

import random, md5, socket, time

def GenerateGUID():
    t = long(time.time() * 1000)
    r = long(random.random() * 100000000000000000L)
    ip = ''
    try:
        ip = socket.gethostbyname(socket.gethostname())
    except:
        # if we can't get a network address, just imagine one
        ip = str(random.random() * 100000000000000000L)
    data = str(t) + ' ' + str(r) + ' ' + ip
    guidStr = md5.new(data).hexdigest()
    # now that we have the GUID format it to look like
    # XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX
    return '%s-%s-%s-%s-%s' % (guidStr[:8], guidStr[8:12], guidStr[12:16], 
                               guidStr[16:20], guidStr[20:])
