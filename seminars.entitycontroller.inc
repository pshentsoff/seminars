<?php

/**
  * @file
  * Seminars entity controller  
  *
  * @author Vadim Pshentsov <pshentsoff@yandex.ru>
  *   
  */  


/**
 * Class SeminarsEntityController
 */
class SeminarsEntityController extends DrupalDefaultEntityController {
  /**
   * Method create empty seminars entity object
   * @return - empty entity object   
   */     
  public function create() {
    global $user;
    
    return (object) array(
      'uid' => $user->uid,
      'created' => REQUEST_TIME,
      'changed' => REQUEST_TIME,
      'status' => 0,
      'promote' => 0,
      'sticky' => 0,
      'title' => '',
      'purpose' => '',
      'content' => '',
      'seminars_masters' => array(
        (object) array(
          'FIO' => '',
          'degrees' => '',
          'photo_fid' => SEMINARS_SEMINARS_MASTERS_PHOTO_NOPHOTO_FID,
          ),
        ),
      'seminars_categories' => array(
        (object) array(
          'title' => '',
          'description' => '',
          ),
        ),
      'seminars_periods' => array(
        (object) array(
          'start_date' => REQUEST_TIME,
          'end_date' => REQUEST_TIME,
          ),
        ),
      );
    }
   
  /**
   * Method saves entity object
   * @param $seminars - saved entity by reference
   * @return - save entity if success, FALSE otherwise      
   */       
  public function save(&$seminars) {
  
    global $user;
    
    try {
      $transaction = db_transaction();
      
      $seminars->is_new = empty($seminars->sid);
      
      if($seminars->is_new) {
        $seminars->created = REQUEST_TIME;
        }
      
      $seminars->changed = REQUEST_TIME;
      $seminars->uid = $user->uid;
      $update_seminars = TRUE;
      
      field_attach_presave('seminars', $seminars);
      
      if($seminars->is_new) {
        drupal_write_record('seminars', $seminars);
        $op = 'insert';
        } else {
        drupal_write_record('seminars', $seminars, 'sid');
        $op = 'update';
        }
        
      $function = 'field_attach_'.$op;
      $function('seminars', $seminars);
      
      //Save seminars path alias if set
      if(!empty($seminars->path_alias)) {
        $uri = seminars_uri($seminars);
      
        //Check unique
        $path = path_load(array('alias' => $seminars->path_alias));
        if($path && ($path['source'] != $uri['path'])) {
          $count = 0;
          $clean = $seminars->path_alias;
          while(path_load(array('alias' => $seminars->path_alias))) {
            $seminars->path_alias = $clean.'-'.$count;
            $count++;
            }
          }

        $path = array(
          'source' => $uri['path'],
          'alias' => $seminars->path_alias,
          'pid' => huf_issetor($seminars->pid),
          'language' => language_default('language'),
          );
          
        path_save($path);
        }
      
      $this->save_masters($seminars);
      $this->save_categories($seminars);
      $this->save_periods($seminars);
      
      module_invoke_all('entity_'.$op, $seminars, 'seminars');
      unset($seminars->is_new);
      
      return $seminars;
      
      } catch (exception $e) {
      $transaction->rollback();
      watchdog_exception('seminars', $e, NULL, WATCHDOG_ERROR);
      return FALSE;
      }
    }
    
    
  /**
   * Protected method save seminars masters data
   * and append or update relations     
   * @param - $seminars - saved entity, by reference
   * @return - no return           
   */         
  protected function save_masters(&$seminars) {
  
    foreach($seminars->seminars_masters as $key => $master) {
    
      field_attach_presave('seminars_masters', $master);
      
      if(empty($master->smid) || $master->smid == 0) {
        drupal_write_record('seminars_masters', $master);
        $op = 'insert';
        } else {
        drupal_write_record('seminars_masters', $master, 'smid');
        $op = 'update';
        }
      
      db_merge('seminars_masters_relations')
        ->key(array('sid' => $seminars->sid,'smid' => $master->smid))
        ->fields(array('sid' => $seminars->sid,'smid' => $master->smid))
        ->execute();
        
      $function = 'field_attach_'.$op;
      $function('seminars_masters', $master);
      
      $seminars->seminars_masters[$key] = $master;
      }
      
  }
    
  /**
   *  Protected method saves seminars categories
   *  and append or update relations
   *  @param $seminars - saved entity by reference
   *  @return - no return
   */                          
  protected function save_categories(&$seminars) {
    
    foreach($seminars->seminars_categories as $key => $category) {
      
      field_attach_presave('seminars_categories', $category);
      
      if(empty($category->scid) || $category->scid == 0) {
        drupal_write_record('seminars_categories', $category);
        $op = 'insert';
        } else {
        drupal_write_record('seminars_categories', $category, 'scid');
        $op = 'update';
        }
      
      db_merge('seminars_categories_relations')
        ->key(array('sid' => $seminars->sid,'scid' => $category->scid))
        ->fields(array('sid' => $seminars->sid,'scid' => $category->scid))
        ->execute();
        
      $function = 'field_attach_'.$op;
      $function('seminars_categories', $category);
      
      // Save path alias if set
      if(!empty($category->path_alias)) {
      
        $uri = seminars_category_uri($category->scid);
        
        //Check unique
        $path = path_load(array('alias' => $category->path_alias));
        if($path && ($path['source'] != $uri['path'])) {
          $count = 0;
          $clean = $category->path_alias;
          while(path_load(array('alias' => $category->path_alias))) {
            $category->path_alias = $clean.'-'.$count;
            $count++;
            }
          }
          
        $path = array(
          'source' => $uri['path'],
          'alias' => $category->path_alias,
          'pid' => huf_issetor($category->pid),  
          'language' => language_default('language'),
          );
        path_save($path);
        }
      
      $seminars->seminars_categories[$key] = $category;
      }
      
    }
  
  /**
   * Protected method saves seminars periods
   * @param $seminars - saved entity by reference
   * @return - no return
   */                     
  protected function save_periods(&$seminars) {
    
    foreach($seminars->seminars_periods as $key => $period) {
    
      $period->sid = $seminars->sid;
    
      field_attach_presave('seminars_periods', $period);
      
      if(empty($period->spid) || $period->spid == 0) {
        drupal_write_record('seminars_periods', $period);
        $op = 'insert';
        } else {
        drupal_write_record('seminars_periods', $period, 'spid');
        $op = 'update';
        }
        
      $function = 'field_attach_'.$op;
      $function('seminars_periods', $period);
      
      $seminars->seminars_periods[$key] = $period;
      }
      
    }
  
  /**
   *  Method load entity data
   *  @param $sid - seminars entity id array
   *  @param $conditions - array if conditions, works but for compatibility only
   *  @param $reset - reset entity cache      
   *  @return - seminars entity object or FAlSE otherwise
   */               
  public function load($sids = array(), $conditions = array(), $reset = FALSE) {
  
    $seminars = parent::load($sids, $conditions, $reset);
    
    foreach($seminars as $key => $seminar) {
    
      //Load path aliases if set
      $uri = seminars_uri($seminars[$key]);
      if($path = path_load($uri['path'])) {
        $seminars[$key]->path_alias = $path['alias'];
        $seminars[$key]->pid = $path['pid'];
        } else {
        $seminars[$key]->path_alias = NULL;
        $seminars[$key]->pid = NULL;
        }
    
      $seminars[$key]->seminars_periods 
        = $this->load_periods($seminar->sid);
      $seminars[$key]->seminars_categories 
        = $this->load_categories($seminar->sid);
      $seminars[$key]->seminars_masters 
        = $this->load_masters($seminar->sid);
        
      }
    
    return $seminars;
    }
  
  /** 
   *  Load seminar periods
   *  @param $sid - {seminars}.sid
   *  @return - array of periods objects or empty array
   */         
  public function load_periods($sid) {
  
    $que = db_select('seminars_periods', 'sp');
    $que->fields('sp');
    $que->condition('sid', $sid);
    $que->orderBy('start_date');
    
    return $que->execute()->fetchAll();
    }
    
  /**
   *  Load seminar categories
   *  @param $sid - {seminars}.sid
   *  @return - array of categories objects or empty array
   */         
  public function load_categories($sid) {
  
    $que = db_select('seminars_categories', 'sc');
    $que->fields('sc');
    $que->join('seminars_categories_relations', 'scr', 'scr.scid = sc.scid');
    $que->condition('scr.sid', $sid);
    
    $categories = $que->execute()->fetchAll();
    
    //Load categories path aliases if set
    foreach($categories as $key => $category) {
      $uri = seminars_category_uri($category->scid);
      if($path = path_load($uri['path'])) {
        $categories[$key]->path_alias = $path['alias'];
        $categories[$key]->pid = $path['pid'];
        }
      }
    
    return $categories;
    }
     
  /**
   *  Load seminar masters
   *  @param $sid - {seminars}.sid
   *  @return - array of masters objects or empty array
   */         
  public function load_masters($sid) {
  
    $que = db_select('seminars_masters', 'sm');
    $que->fields('sm');
    $que->join('seminars_masters_relations', 'smr', 'smr.smid = sm.smid');
    $que->condition('smr.sid', $sid);
  
    return $que->execute()->fetchAll();
    
    } 
  
  /**
   * Method delete entity
   * @param $sid - {seminars}.id to delete
   * @return - no return
   */             
  public function delete($sid, $delete_categories = FALSE, $delete_masters = FALSE) {
  
    // First of all - delete categories and masters if needed
    // while relations not deleted
    if($delete_categories) {
      $this->delete_categories($sid);
      }
    if($delete_masters) {
      $this->delete_masters($sid);
      }
    
    // Delete seminar, periods and relations    
    db_delete('seminars')->condition('sid', $sid)->execute();
    db_delete('seminars_periods')->condition('sid', $sid)->execute();
    db_delete('seminars_categories_relations')->condition('sid', $sid)->execute();
    db_delete('seminars_masters_relations')->condition('sid', $sid)->execute();
        
    }
    
  /**
   * Method delete categories for given {seminars}.sid
   * @param $sid - deleted entity sid
   * @return - no return
   */
  public function delete_categories($sid) {
         
    $que = db_select('seminars_categories_relations', 'scr');
    $que->fields('scr', array('scid'));
    $que->condition('sid', $sid);
    
    $scids_res = $que->execute()->fetchAllAssoc();
    $scids = array_values($scids_res);
    
    if(count($scids)) {
      db_delete('seminars_categories')
        ->condition('scid', $scids, 'IN')
        ->excecute();
      }
      
    } 
  
  /**
   * Method delete masters for given {seminars}.sid
   * @param $sid - deleted entity sid
   * @return - no return
   */             
  public function delete_masters($sid) {
  
    $que = db_select('seminars_masters_relations', 'smr');
    $que->fields('smr', array('smid'));
    $que->condition('sid', $sid);
    
    $smids_res = $que->execute()->fetchAllAssoc();
    $smids = array_values($smids_res);
    
    if(count($smids)) {
      db_delete('seminars_masters')
        ->condition('smid', $smids, 'IN')
        ->execute();
      }
        
    }
    
  /**
   * Delete period with given id
   * @param $sid - seminar id
   * @param $spid - seminars period id to delete
   * @return - TRUE if succ, FALSE otherwise 
   * @since 7.x-0.5.0-dev 
   **/
  public function delete_period($sid, $spid) {
  
    $que = db_delete('seminars_periods');
    $que->condition('spid', $spid);
    $que->condition('sid', $sid);
    $res = $que->execute();
    
    return ($res > 0);
    }
   
  /**
   * Remove category with given id
   * @param $sid - seminar id
   * @param $scid - seminar category id to remove
   * @return - TRUE if succ, FALSE otherwise 
   * @since 7.x-0.5.0-dev  
   **/
  public function remove_category($sid, $scid) {
    
    $que = db_delete('seminars_categories_relations');
    $que->condition('scid', $scid);
    $que->condition('sid', $sid);
    $res = $que->execute();
    
    return ($res > 0);  
  }
  
  /**
   * Remove master with given id
   * @param $sid - seminar id
   * @param $smid - seminar master id to remove
   * @return - TRUE if succ, FALSE otherwise 
   * @since 7.x-0.5.0-dev 
   **/
  public function remove_master($sid, $smid) {
    
    $que = db_delete('seminars_masters_relations');
    $que->condition('smid', $smid);
    $que->condition('sid', $sid);
    $res = $que->execute();
    
    return ($res > 0);  
  }
} 