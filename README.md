pega-texto-debugger
===================
Debugging facilities for pega-texto.

This API provides a `pt_match_options` template to be passed to `pt_match*`
functions, which open a debugger REPL for the matching call. You can create the
debugger and get a reusable `pt_match_options` or create a one shot
`pt_match_options` that takes care of allocating and freeing the debugger.
