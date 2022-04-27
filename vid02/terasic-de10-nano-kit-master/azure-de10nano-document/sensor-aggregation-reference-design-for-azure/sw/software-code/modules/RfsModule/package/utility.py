from logging import getLogger, StreamHandler, DEBUG

'''
----------------------------------------------
General Definitions
----------------------------------------------
'''
logger = getLogger(__name__)
handler = StreamHandler()
handler.setLevel(DEBUG)
logger.setLevel(DEBUG)
logger.addHandler(handler)
logger.propagate = False
