<?php

/**
  * @file
  * Seminars module page template
  * 
  * @author Vadim Pshentsov <pshentsoff@yandex.ru>
  *   
  * Variables:      
  * $seminars - Seminars entity, prepared:
  *   $seminars->seminars_masters[]->photo_file - linkpath to master's photo
  *   $seminars->periods[]->start_date_str - translated start dates 
  *   $seminars->periods[]->end_date_str - translated end dates
  *   $seminars->content_origin - keeped origin seminars content (filtered html)      
  * $date - formatted string date seminars entity created 
  * $name - creator's account name with link to profile page
  * $seminars_url - url to current seminars (for 'read more' link as simple)
  * $read_more - translated 'Read more...' text link to seminars url  
  * $title - seminars title
  * $view_mode - view mode: teaser, full, block  
  * $page - is full page
  * 
  * Translated and simply text constants:    
  * $title_seminar - translated 'Seminar' title
  * $title_periods - translated 'Periods' title
  * $title_direction - translated 'Direction' title
  * $title_master - translated 'Master' title
  * $title_content - translated 'Content' title
  * $title_diplomas - translated 'Participants recieve' title  
  * $title_request - translated 'Request' title  
  *   
  */

?>
<?php if($page): ?>

<div id='citata'>
<p><?php print $seminars->purpose; ?></p>
</div>                          
                                                      
<h2><?php print $title_seminar; ?></h2>
<h1><?php print $seminars->title; ?></h1>

<p><span><?php print $title_periods; ?></span>
<?php 
foreach($seminars->seminars_periods as $period) {
  print '<p>'.$period->start_date_str.' - '.$period->end_date_str.'</p>';
  } ?></p>

<p><span><?php print $title_direction; ?></span>
<?php foreach($seminars->seminars_categories as $category) {
  print $category->title;
  } ?>
</p>

<p><span><?php print $title_master; ?></span>
<?php foreach($seminars->seminars_masters as $master) { ?>
<div id='autor'>
  <img alt="" src="<?php print $master->photo_file; ?>" style="float: left;">
  <p><?php print $master->fio; ?></p>
  <p><?php print render($master->degrees); ?></p>
</div>
<?php } ?>
</p>

<p><span><?php print $title_content; ?></span>
<?php print render($seminars->seminars_content); ?>
</p>

<p><span><?php print $title_diplomas; ?></span></p>
<p><?php print render($seminars->diplomas); ?></p>

<a href="<?php print base_path().'seminars/request/'.$seminars->sid; ?>">
<img border="0" src="<?php print base_path().drupal_get_path('theme', 'iamme'); ?>/images/zayavkat.gif"></a>
<p>&nbsp;</p>
  
<?php elseif($view_mode == 'teaser'): ?>

<div  style="float: left"><p>
<?php 
foreach($seminars->seminars_periods as $period) {
  print '<p>'.$period->start_date_str.' - '.$period->end_date_str.'</p>';
  } ?>
</p></div>
<div>
<?php print render($title_prefix); ?>
<a href="<?php print $seminars_url; ?>" style="text-decoration: none;">
<h1><?php print $seminars->title; ?></h1></a>
<?php print render($title_suffix); ?>
<p>
<?php foreach($seminars->seminars_categories as $category) {
  print $category->title;
  } ?>
</p>
</div>
<p>&nbsp;</p>

<?php elseif($view_mode == 'list'): ?>

  <?php if(isset($seminars->type) && $seminars->type == 'title'): ?>
    <h2><?php print $seminars->title; ?></h2>
    <div id='citata'>
    <p><?php print $seminars->description; ?></p>
    </div>                          
  <?php else : ?>                                                    
    <div style="clear:left;">
    <div  style="width: 275px; float: left; padding-top: 0.75em; word-wrap: break-word;">
    <?php print render($title_prefix); ?>
    <a href="<?php print $seminars_url; ?>" style="text-decoration: none;">
    <h1><?php print $seminars->title; ?></h1></a>
    <?php print render($title_suffix); ?>
    </div>
    <div  style="float: left; width: 200px;"><p>
    <?php 
    if(count($seminars->seminars_periods)) {
      foreach($seminars->seminars_periods as $period) {
        print '<p>'.$period->start_date_str.' -<br />'.$period->end_date_str.'</p>';
        } 
      } else {
      print '<p>&nbsp;</p>';
      }
    ?>
    </p></div>
    <div style="padding-top: 0.75em;"><p>
    <a href="<?php print base_path().'seminars/request/'.$seminars->sid; ?>">
    <?php print $title_request; ?></a>
    </p></div>
    <p>&nbsp;</p>
    </div>
  <?php endif ?>
<?php endif; ?>