O que um debugger pro *pega-texto* precisa ter?
-----------------------------------------------
- Inspeção da gramática
- Inspeção da *string* sendo *matcheada*
- Inspeção da pilha de `pt_match_state` e estados individuais
- Inspeção da pilha de `pt_match_action` e ações individuais
- *Breakpoints* por nome da regra
- Ajuda e tutorial

Extras (se rolar fazer, sucesso)
--------------------------------
- *Breakpoints* por expressão
  + **Problema:** como mapear expressões individuais (com o aninhamento que
    houver) em strings? *Talvez atribuir índices numéricos em runtime na hora
	de escolher*
- Interface de CLI usando `readline` ou afins (`replxx`, estou de olho em você)
  + Compleção de comandos
  + Histórico de comandos
- Poder trocar a gramática *on-the-fly*
  + Incluir voltar ao que era antes
  + Tomar cuidado com memória!



Comandos importantes
--------------------
- help
- step
- next
- continue
- finish
- backtrace (estado)
- list
- print 
- rules (printa as regras da gramática)
- breakpoint

outros comandos:
- frame N
- up/down

