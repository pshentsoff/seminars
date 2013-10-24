<?php

/**
  * @file
  * Seminars Entity  
  *
  * @author Vadim Pshentsov <pshentsoff@yandex.ru>
  *   
  */  


/**
 * Entity uri callback.
 */
function seminars_uri($seminars) {
  if(isset($seminars) && !empty($seminars) && is_object($seminars)) {
    return array(
      'path' => 'seminars/' . $seminars->sid,
      );
    } else {
    return FALSE;
    }
}

/**
 * Categories uri callback
 */
function seminars_category_uri($scid) {
  return array(
    'path' => 'seminars/category/'.$scid,
    );
} 

/**
 *  Implements hook_entity_info()
 */
function seminars_entity_info() {

  $entity_info = array(
    'seminars' => array(
      'label' => t('seminars'),
      'controller class' => 'SeminarsEntityController', 
      'base table' => 'seminars',
      //'revision table' => 'seminars_revision',
      'static cache' => TRUE,
      'uri callback' => 'seminars_uri',
      'load hook' => 'seminars_load',
      'access callback' => 'seminars_access',
      'fieldable' => TRUE,
      'entity keys' => array(
        'id' => 'sid',
        //'revision' => 'vid',
        'label' => 'title',
#        'bundle' => 'type',
        ),
      // for field_extract_bundle($entity_type, $bundle) function:
#      'bundle keys' => array(
#        'bundle' => 'type',
#        ),
#      'bundles' => array(),
      ),
    );
    
  return $entity_info;
}

/**
 * Check access rights
 */
#function seminars_access($op, $seminars = NULL, $account = NULL, $entity_type = 'seminars') {
function seminars_access($op, $account = NULL) {

  global $user;
  $access = FALSE;
  
  $account = isset($account) ? $account : $user;
  
  if($op == 'create' || $op == 'edit' || $op == 'delete') {
    $access = user_access('create, edit, delete seminars', $account);
    } else {
    $access = user_access($op.' seminars', $account);
    }
    
  return $access; 
}

/**
 * Entity wrapper functions
 */
 
/**
 * Save entity
 * @param $seminars - saved 
 * @return - saved seminars entity or FALSE  
 **/ 
function seminars_save($seminars) {
  return entity_get_controller('seminars')->save($seminars);
}

/**
 * Return Seminar by ID
 * Controller wrapper function 
 * @param $sid - seminar id to load
 * @param $conditions - placed for compatibility purposes
 * @param $reset - reset or not entity cache
 * @return - loaded entity    
 */
function seminars_load($sid = FALSE, $conditions = array(), $reset = FALSE) {

  $sids = (isset($sid) ? array($sid) : FALSE);
  
  $seminars = seminars_load_multiple($sids, array(), $reset);
  
  return $seminars ? reset($seminars) : FALSE;
}

/**
 * Return Seminars array by ID array
 * Controller wrapper function
 * @param $sids - array of seminar id to load
 * @param $conditions - placed for compatibility purposes
 * @param $reset - reset or not entity caches
 * @return - loaded entities array   
 */  
function seminars_load_multiple($sids = FALSE, $conditions = array(), $reset = FALSE) {
  $seminars = entity_load('seminars', $sids, array(), $reset);
  reset($seminars);
  return $seminars;
}

/**
 * Delete seminar
 *  controller wrapper
 */
function seminars_delete($sid, $delete_categories = FALSE, $delete_masters = FALSE) {
  static $sctrl;
  if(empty($sctrl)) $sctrl = entity_get_controller('seminars');
  $sctrl->delete($sid, $delete_categories, $delete_masters);
} 

/**
 * Delete multiple seminars 
 * controller wrapper
 */
function seminars_delete_multiple($sids, $delete_categories = FALSE, $delete_masters = FALSE) {

  foreach($sids as $sid)
    seminars_delete($sid, $delete_categories, $delete_masters);
  
}  

function seminars_load_master($smid) {

  $que = db_select('seminars_masters', 'sm');
  $que->fields('sm');
  $que->condition('sm.smid', $smid);

  return $que->execute()->fetchAll();
  
} 

function seminars_get_master_photo($photo_fid) {
  if(isset($photo_fid) && !empty($photo_fid)) {
    $photo = file_create_url(file_load($photo_fid)->uri);
    } else {
    $photo = base_path().SEMINARS_SEMINARS_MASTERS_PHOTO_NOPHOTO_FILE;
    }
  return $photo; 
}

function seminars_load_category($scid) {

  $que = db_select('seminars_categories', 'sc');
  $que->fields('sc');
  $que->condition('sc.scid', $scid);
  
  return $que->execute()->fetchAll();
}

function seminars_get_periods_sids($periods) {

  $where_values = array();
  if(isset($periods['begin_period'])) {
    $where_begin = ' start_date >= :begin_begin AND start_date <= :begin_end ';
    $where_values[':begin_begin'] = $periods['begin_period']['begin']; 
    $where_values[':begin_end'] = $periods['begin_period']['end']; 
    }
  if(isset($periods['end_period'])) {
    $where_end = ' end_date >= :end_begin AND end_date <= :end_end ';
    $where_values[':end_begin'] = $periods['end_period']['begin']; 
    $where_values[':end_end'] = $periods['end_period']['end']; 
    }
    
  $que = 'SELECT sid FROM {seminars_periods}';
  if(isset($where_begin) || isset($where_end)) {
    $que .= ' WHERE';
    $que .= isset($where_begin) ? $where_begin : '';
    if(isset($where_begin) && isset($where_end))
      $que .= ' AND '.$where_end;
    }
  $que .= ' ORDER BY start_date';
  
  //DEBUG:
  //watchdog('seminars', 'seminars_get_periods_sids(): $que = '.dvr($que, TRUE)); 
  
  $res = db_query($que, $where_values);
  return $res->fetchCol();
                          
}

function seminars_get_from_date($start_date = NULL) {
  $start_date = isset($start_date) ? $start_date : time();
  $que = 'SELECT sid FROM {seminars_periods}';
  $que .= ' WHERE start_date >= :start_date';
  $que .= ' ORDER BY start_date';
  $res = db_query($que, array(':start_date' => $start_date));
  return $res->fetchCol();
}

function seminars_seminar_category_id($scid, array $seminars) {
  $result = FALSE;
  foreach($seminars as $seminar) {
    if(!count($seminar->seminars_categories)) continue;
    foreach($seminar->seminars_categories as $category) {
      if($category->scid == $scid) {
        return $seminar;
        }
      }
    }
  return $result;
}

/**
 * Delete period with given id
 * @param $seminars - seminar entity
 * @param $spid - seminar period id to delete
 * @return - TRUE if succ, FALSE otherwise
 * @since 7.x-0.5.0-dev  
 **/
function seminars_delete_period(&$seminars, $spid) {
  $result = FALSE;
  if(!empty($seminars->seminars_periods)) {
    foreach($seminars->seminars_periods as $id => $period) {
      if(!empty($period) && ($period->spid == $spid)) {
        $result = entity_get_controller('seminars')->delete_period($seminars->sid, $spid);
        unset($seminars->seminars_periods[$id]);
        break;
        }
        
      }
    }    
  return $result; 
}
  
/**
 * Remove category with given id
 * @param $sid - seminar id
 * @param $scid - seminar category id to remove
 * @return - TRUE if succ, FALSE otherwise 
 * @since 7.x-0.5.0-dev  
 **/
function seminars_remove_category($seminars, $scid) {
  $result = FALSE;
  if(!empty($seminars->seminars_categories)) {
    foreach($seminars->seminars_categories as $id => $category) {
      if(!empty($category) && ($category->scid == $scid)) {
        $result = entity_get_controller('seminars')->remove_category($seminars->sid, $scid);
        unset($seminars->seminars_categories[$id]);
        break;
        }
      }
    }
  return $result;
}

/**
 * Remove master with given id
 * @param $sid - seminar id
 * @param $smid - seminar master id to remove
 * @return - TRUE if succ, FALSE otherwise 
 * @since 7.x-0.5.0-dev 
 **/
function seminars_remove_master($seminars, $smid) {
  $result = FALSE;
  if(!empty($seminars->seminars_masters)) {
    foreach($seminars->seminars_masters as $id => $master) {
      if(!empty($master) && ($master->smid == $smid)) {
        $result = entity_get_controller('seminars')->remove_master($seminars->sid, $smid);
        unset($seminars->seminars_masters[$id]);
        break;
        }
      }
    }
  return $result;
}
    