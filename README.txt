
CONTENTS OF THIS FILE
---------------------

 * Introduction
 * Features
 * Requirements
 * Installation & Upgrading
 * Maintainers


INTRODUCTION
------------

The Birthdays module allows users to add their birthday to their profile. In
their profile the date of birth can be shown, as well as their age and their
star sign. This is all configurable.

You can also list the birthdays on blocks and pages using Views. You can filter
by day. month and year, display only N upcoming birthdays and so on.

It is optional to send users an email or execute another action on their
birthday, and the administrator can receive periodic reminders of who are having
their birthday next day, week or month.


FEATURES
--------

 * Provides a field type that can be used on users, profiles or whatever entity
   you want.
 * Display a birthday input field on registration (or not).
 * A block displaying the next X birthdays, using Views.
 * A block listing birthdays for the next X days, using Views.
 * A page listing all the users and their birthdays/age/star sign, filtered by
   month and year, using Views.
 * Displaying the user's birthday/current age/star sign on the user's profile.
 * User option not to be mailed on their birthday.
 * User & admin option to hide the year and age of the user(s).
 * User options can be turned off by administrator.
 * Optionally send e-mails to administrator with upcoming birthdays for the
   next day, week or month.
 * Optionally sends an email to the user on their birthday. Other actions are
   also possible because Birthdays integrates with the Triggers module.
 * PGSQL support


REQUIREMENTS
------------

This module requires only the Field module that is included in Drupal core, but
it's functionality can be extended using Views, Triggers, Profile 2 and other
contrib modules.


INSTALLATION & UPGRADING
------------------------

See http://drupal.org/documentation/install/modules-themes/modules-7
for instrunctions on installing contrib modules.

Make sure you read UPGRADE.txt before upgrading from Drupal 6.


MAINTAINERS
-----------

 * David Gildeh (Drazzig) - http://drupal.org/user/26260
 * Maarten van Grootel (maartenvg) - http://drupal.org/user/109716
 * Niklas Fiekas - http://drupal.org/user/1089248 - niklas.fiekas@googlemail.com
