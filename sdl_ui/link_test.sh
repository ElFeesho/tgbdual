#!/bin/bash

tmux new-session -d './tgbdual -s crystal.gbc'
tmux split-window -h './tgbdual -c localhost gold.gbc'
tmux -2 attach-session -d 
