#!/usr/bin/env python3

import re
import os
import json

def check_commit_message(msg: str) -> re.match:
  if not msg:
    return None
  match = re.search(r'^\[(KS|IL|MI|ST|MA)-(D|C)\]', msg)
  if not match:
    return re.search(r'^\[KS-(master|tests)\]', msg)

def main():
  commits = os.getenv("COMMITS")
  if not commits:
    return 1
  
  messages = [entry["message"] for entry in json.loads(commits)]
  for msg in messages:
    if not check_commit_message(msg):
      print("Commit message must begin with your [init letters] and continue with [C for Client] or [D for Daemon] or [master] or [tests]")
      return 1
  print("All commits messages are correct")
  

main()