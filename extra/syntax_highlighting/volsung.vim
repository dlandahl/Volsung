" Vim syntax file for the Volsung audio generation and processing language
" landahl.tech/volsung

if exists("b:current_syntax")
  finish
endif

syn match vlsng_arrow "\(->\|=>\|<>\|x>\|>>\|-->\)"
syn match vlsng_number "\d\+\(\.\d\+\)\?"
syn match vlsng_comment ";.*$"
syn match vlsng_object "\w\+\~"
syn match vlsng_string '\".*\"'

hi def link vlsng_arrow   Keyword
hi def link vlsng_number  Constant
hi def link vlsng_comment Comment
hi def link vlsng_object  Type
hi def link vlsng_string  String

let b:current_syntax = 'volsung'

