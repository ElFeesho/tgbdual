#!/bin/bash

tmux new-session -d './tgbdual -s crystal.gbc > server-`date +%s`.bin'
tmux split-window -h './tgbdual -c localhost crystal.gbc > client-`date +%s`.bin'
tmux -2 attach-session -d 
