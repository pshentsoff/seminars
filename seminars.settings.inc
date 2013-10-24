<?php

/**
  * @file
  * Seminars settings  
  *
  * @author Vadim Pshentsov <pshentsoff@yandex.ru>
  *   
  */  

define('SEMINARS_TEXTFIELD_SIZE_SMALL', 25);
define('SEMINARS_TEXTFIELD_SIZE_MEDIUM', 60);
define('SEMINARS_TEXTFIELD_SIZE_LARGE', 120);
//TODO: change to normal "no photo" file
define('SEMINARS_SEMINARS_MASTERS_PHOTO_NOPHOTO_FILE', 'images/autor/no_photo.jpg');
define('SEMINARS_SEMINARS_MASTERS_PHOTO_PATH', 'images/autor/');
define('SEMINARS_SEMINARS_MASTERS_PHOTO_WIDTH', 100);
define('SEMINARS_SEMINARS_MASTERS_PHOTO_HEIGHT', 100);

/**
 * Tables settings
 */
define('SEMINARS_SEMINARS_TITLE_LENGTH', 256);
define('SEMINARS_SEMINARS_CONTENT_FORMAT_LENGTH', 128);
define('SEMINARS_SEMINARS_CONTENT_FORMAT_DEFAULT', 'full_html');
define('SEMINARS_SEMINARS_MASTERS_FIO_LENGTH', 256);
define('SEMINARS_SEMINARS_CATEGORIES_TITLE_LENGTH', 256);

/**
 * Other settings
 */
define('SEMINARS_PLANS_MONTHS', 2); #without current month
define('SEMINARS_MENU_SEMINARS_DIRECTIONS', 'menu-seminars-directions');
define('SEMINARS_WEBFORM_REQUEST_FORM_ID', 5);
#define('SEMINARS_WEBFORM_REQUEST_FORM_ID', 7);   