#!/bin/bash

for f in 587 588 589 590 591 592 593 594 595 598 600 601 602 604 605 606 607 612 615 617 619 621 624 626 627 628 629 630
 do
  echo "./fixLostTriggers $f > buggedRunList/sps2021data.run${f}.new" | sh -
 done
