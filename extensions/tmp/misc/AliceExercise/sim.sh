#!/bin/bash

ENV=environment.xml
FIT=fitness.js
NET=network.onn
VALS=values.val
EVO_PARAMS=evoParameters.val
ORCS=../OrcsEvaluation


exec "${ORCS}" -env "${ENV}" -net Alice "${NET}" -net BadGuy1 badGuyNet.onn -net BadGuy2 badGuyNet.onn -net Friend1 friendNet.onn -net Friend2 friendNet.onn -fit Alice Script Script -fsf Script "${FIT}" -gui -val "${VALS}" -val "${EVO_PARAMS}" -val selectionParameters.val -val essentials.val "$@"

