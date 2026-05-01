#!/usr/bin/env python3

import re
import sys
import os

def check_pr_title(title: str) -> re.match:
  if not title:
    return None
  return re.search(r'^\[(client|deamon|master|tests)\]', title)

def main():
  title = str(os.getenv("PR_TITLE"))

  if not title:
    print("Incorrect script arguments")
    sys.exit(1)

  if not check_pr_title(title):
    print("Pr title must begin with [client], [daemon], [master] or [tests]")
    sys.exit(1)
  print("Pr title is correct")


main()
