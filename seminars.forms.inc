<?php

/**
  * @file
  * Seminars main forms include  
  *
  * @author Vadim Pshentsov <pshentsoff@yandex.ru>
  *   
  */  
  
require_once('seminars.settings.inc');

function seminars_form_categories_set(&$form, &$form_state, $category, $count) {
  
  $scid = huf_issetor($category->scid, 0);
  $new_title = t('New category');
  $fsid = 'category-'.$count;
  if(!isset($categories_titles)) {
    $categories_titles = seminars_categories_get_titles();
    }
  
  $form['categories'][$fsid] = array(
    '#type' => 'fieldset',
    '#tree' => TRUE,
    '#title' => t('[@number] '.(!empty($category->title) ? $category->title : $new_title),
        array('@number' => ($count+1))),
    '#collapsible' => TRUE,
    '#collapsed' => ($scid == 0 ? FALSE : TRUE),
    );
  $form['categories'][$fsid]['scid'] = array(
    '#type' => 'hidden',
    '#value' => $scid,
    );
  $form['categories'][$fsid]['selected'] = array(
    '#type' => 'checkbox',
    '#prefix' => '<div style="float: left; padding-top: 0px; padding-right: 20px;">',
    '#suffix' => '</div>',
    ); 
  $form['categories'][$fsid]['title-markup']['#markup'] = 
    t('You can select existent category or enter new.'); 
  $form['categories'][$fsid]['title-select'] = array(
    '#type' => 'select',
    '#title' => t('Categories list'),
    '#options' => $categories_titles,
    '#default_value' => $scid,
    '#empty_option' => t(' --- Choose category --- '),
    '#attributes' => array(
      'onchange' => "seminarsCategoriesTitleSelectChange('$fsid');",
      ),
    );
  $form['categories'][$fsid]['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#maxlentgh' => SEMINARS_SEMINARS_TITLE_LENGTH,
    '#size' => SEMINARS_TEXTFIELD_SIZE_LARGE,
    '#default_value' => huf_issetor($category->title, $new_title),
    '#attributes' => array(
      'onchange' => "seminarsCategoriesTitleTextChange('$fsid');",
      ),
    '#required' => TRUE,
    );
    
  //Path alias
  $form['categories'][$fsid]['path-alias-group-begin']['#markup'] = '<div>';
  $form['categories'][$fsid]['pid'] = array(
    '#type' => 'value',
    '#value' => huf_issetor($category->pid),
    );
  $form['categories'][$fsid]['path-alias'] = array(
    '#type' => 'textfield',
    '#title' => t('URL alias'),
    '#maxlentgh' => SEMINARS_SEMINARS_TITLE_LENGTH,
    '#size' => SEMINARS_TEXTFIELD_SIZE_MEDIUM,
    '#default_value' => huf_issetor($category->path_alias),
    '#prefix' => '<div id="'.$fsid.'-path-alias-ajax-wrapper" style="float: left; margin-right: 25px;">',
    '#suffix' => '</div>',
    );
  $form['categories'][$fsid]['path-alias-request'] = array(
    '#type' => 'button',
    '#name' => $fsid,
    '#value' => t('Title translit'),
    '#validate' => array(),
    '#limit_validation_errors' => array(),
    '#ajax' => array(
      'callback' => 'seminars_form_ajax_category_path_alias',
      'wrapper' => $fsid.'-path-alias-ajax-wrapper',
      'method' => 'replace',
      ),
    '#prefix' => '<div style="padding-top: 25px;">',
    '#suffix' => '</div>',
    );
  $form['categories'][$fsid]['path-alias-group-end']['#markup'] 
    = '</div><div style="clear: left;"></div>';
  //If it's AJAX call
  if(isset($form_state['input']['_triggering_element_name'])
    && $form_state['input']['_triggering_element_name'] == $fsid) {
    $translit_path = seminars_form_get_translit_path($form_state['input']['categories'][$fsid]['title']);
    $form['categories'][$fsid]['path-alias']['#default_value'] = $translit_path;
    $form_state['input']['categories'][$fsid]['path-alias'] = $translit_path;
    } 
  
  $form['categories'][$fsid]['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#description' => t('Plain text only.'),
    '#default_value' => huf_issetor($category->description, NULL),
    );

}

function seminars_form_masters_set(&$form, &$form_state, $master, $count) {

  $fio = huf_issetor($master->fio, '');
  $smid = huf_issetor($master->smid, 0);
  $fsid = 'master-'.$count;
  
  $form['masters'][$fsid] = array(
    '#type' => 'fieldset',
    '#tree' => TRUE,
    '#title' => t('[@number] '.(!empty($fio) ? $fio : t('New master')), 
                    array('@number' => ($count+1))),
    '#collapsible' => TRUE,
    '#collapsed' => ($smid == 0 ? FALSE : TRUE),
    );
  $form['masters'][$fsid]['smid'] = array(
    '#type' => 'hidden',
    '#value' => $smid,
    );
  $form['masters'][$fsid]['selected'] = array(
    '#type' => 'checkbox',
    '#prefix' => '<div style="float: left; padding-top: 0px; padding-right: 20px;">',
    '#suffix' => '</div>',
    ); 
  $form['masters'][$fsid]['fio-markup']['#markup'] = 
    t('You can enter new master or select from existent'); 
  $form['masters'][$fsid]['fio-select'] = array(
    '#type' => 'select',
    '#title' => t('Masters list'),
    '#options' => seminars_masters_get_FIO(),
    '#default_value' => $smid,
    '#empty_option' => t(' --- Select master --- '),
    '#attributes' => array(
      'onchange' => "seminarsMastersFIOSelectChange('$fsid')",
      ),
    );
  $form['masters'][$fsid]['fio'] = array(
    '#type' => 'textfield',
    '#default_value' => $fio,
    '#maxlength' => SEMINARS_SEMINARS_MASTERS_FIO_LENGTH,
    '#size' => SEMINARS_TEXTFIELD_SIZE_LARGE,
    '#description' => 
      t('Seminar master FIO <em>(Length @max symbols maximum)</em>', 
        array('@max' => SEMINARS_SEMINARS_MASTERS_FIO_LENGTH)), 
    '#attributes' => array(
      'onchange' => "seminarsMastersFIOTextChange('$fsid')",
      ),
    '#required' => TRUE,
    ); 
  $form['masters'][$fsid]['photo']['#markup'] = 
    '<img id="masters-'.$fsid.'-photo" src="'
      .seminars_get_master_photo(huf_issetor($master->photo_fid))
      .'" >';
  $form['masters'][$fsid]['photo_fid'] = array(
    '#title' => t('Image'),
    '#type' => 'managed_file',
    '#description' => 
      t('The uploaded image will be displayed on this page using the image style choosen below.'),
    '#default_value' => 
      huf_issetor($master->photo_fid, 0),
    '#upload_location' => 'public://'.SEMINARS_SEMINARS_MASTERS_PHOTO_PATH,
    );
  
  $form['masters'][$fsid]['degrees'] = array(
    '#type' => 'textarea',
    '#title' => t('Degrees'),
    '#description' => t('Filtered HTML.'),
    '#default_value' => huf_issetor($master->degrees, NULL),
    );

}

function seminars_form_periods_set(&$form, &$form_state, $period, $count) {
  
  $spid = huf_issetor($period->spid, 0);
  $fsid = 'period-'.$count;
  $period_str = 'New period';
  $sd_array = array();                   
  if($sd = huf_issetor($period->start_date, 0)) {  
    $sd_array['day'] = (int)date('d', $sd);
    $sd_array['month'] = (int)date('m', $sd);
    $sd_array['year'] = (int)date('Y', $sd);
    $period_str = date('d/m/Y', $sd);
    } 
  $ed_array = array();
  if($ed = huf_issetor($period->end_date, 0)) {  
    $ed_array['day'] = (int)date('d', $ed);
    $ed_array['month'] = (int)date('m', $ed);
    $ed_array['year'] = (int)date('Y', $ed);
    $period_str .= ' - '.date('d/m/Y', $ed);
    } 
  
  $form['periods'][$fsid] = array(
    '#tree' => TRUE,
    '#type' => 'fieldset',
    '#title' => t('[@number] '.$period_str,
        array('@number' => ($count+1))),
    '#collapsible' => TRUE,
    '#collapsed' => ($spid == 0 ? FALSE : TRUE),
    );
  $form['periods'][$fsid]['spid'] = array(
    '#type' => 'hidden',
    '#value' => $spid,
    );
  
  $form['periods'][$fsid]['selected'] = array(
    '#type' => 'checkbox',
    '#prefix' => '<div style="float: left; padding-top: 10px;">',
    '#suffix' => '</div>',
    ); 
  $form['periods'][$fsid]['start-date'] = array(
    '#type' => 'date',
    '#default_value' => $sd_array,
    '#prefix' => '<div style="float: left; margin-left: 20px;">',
    '#suffix' => '</div>',
    ); 
  $form['periods'][$fsid]['end-date'] = array(
    '#type' => 'date',
    '#default_value' => $ed_array,
    '#prefix' => '<div style="float: left; margin-left: 20px;">',
    '#suffix' => '</div>',
    );
    
}

/***************************************************************
 *                Main seminars form build
 ***************************************************************/ 
function seminars_form($form, &$form_state, $seminars = NULL, $op = 'edit') {
  
  global $user;
  $date_format = 'd-m-Y H:i:s';
  
  //DEBUG:
  #if($seminars) $form['debug_seminars']['#markup'] = kpr($seminars, TRUE);
  //DEBUG:
  #$form['debug_form_state']['#markup'] = kpr($form_state, TRUE);
    
  #get AJAX trigger
  $trigger = seminars_form_ajax_get_trigger(
    huf_issetor($form_state['triggering_element']));
 
  #Count categories
  if(!isset($form_state['input']['categories']['count'])) {  
    $categories_count = isset($seminars->seminars_categories)
      ? count($seminars->seminars_categories) : 0;
    } else {
    $categories_count = $form_state['input']['categories']['count'];
    }
  #Count masters
  if(!isset($form_state['input']['masters']['count'])) {
    //First init (not ajax call from form)
    $masters_count = isset($seminars->seminars_masters)
      ? count($seminars->seminars_masters) : 0;
    } else {
    $masters_count = $form_state['input']['masters']['count'];
    }
  #Count periods    
  if(!isset($form_state['input']['periods']['count'])) {
    $periods_count = isset($seminars->seminars_periods) 
      ? count($seminars->seminars_periods) : 0;
    } else {
    $periods_count = $form_state['input']['periods']['count'];
    }
  
  
  #Add sets counts if it's ajax call
  if($trigger->ajax && $trigger->action == 'append') {
    
    if($trigger->set == 'period') {
      $periods_count++;
      } elseif($trigger->set == 'category') {
      $categories_count++;
      } elseif($trigger->set == 'master') {
      $masters_count++;
      }
      
    }

  $form['sid'] = array(
    '#type' => 'value',
    '#value' => huf_issetor($seminars->sid, 0),
    );
  $form['uid'] = array(
    '#type' => 'value',
    '#value' => huf_issetor($seminars->uid, $user->uid),
    );
    
  $form['title'] = array(
    '#title' => t('Seminar title'),
    '#description' 
      => t('Enter seminar title <em>(maximum @length symbols length)</em>',
        array('@length' => SEMINARS_SEMINARS_TITLE_LENGTH)),
    '#type' => 'textfield',
    '#maxlength' => SEMINARS_SEMINARS_TITLE_LENGTH,
    '#size' => SEMINARS_TEXTFIELD_SIZE_LARGE,
    '#required' => TRUE,
    '#default_value' => huf_issetor($seminars->title, t('New seminar')),
    );
  
  $form['path-alias-group-begin']['#markup'] = '<div>';  
  // Path alias
  $form['pid'] = array(
    '#type' => 'value',
    '#value' => huf_issetor($seminars->pid),
    ); 
  $form['path-alias'] = array(
    '#title' => t('URL alias'),
    '#description' => t('Enter seminar URL alias'),
    '#type' => 'textfield',
    '#maxlength' => SEMINARS_SEMINARS_TITLE_LENGTH,
    '#size' => SEMINARS_TEXTFIELD_SIZE_MEDIUM,
    '#default_value' => huf_issetor($seminars->path_alias, NULL),
    '#prefix' => '<div id="seminar-path-alias-ajax-wrapper" style="float: left; margin-right: 25px;">',
    '#suffix' => '</div>',
    );
  $form['path-alias-request'] = array(
    '#type' => 'button',
    '#value' => t('Title translit'),
    '#validate' => array(),
    '#limit_validation_errors' => array(),
    '#ajax' => array(
      'callback' => 'seminars_form_ajax_seminar_path_alias',
      'wrapper' => 'seminar-path-alias-ajax-wrapper',
      'method' => 'replace',
      ),
    '#prefix' => '<div style="padding-top: 25px;">',
    '#suffix' => '</div>',
    );
  $form['path-alias-group-end']['#markup'] = '</div><div style="clear: left;"></div>';  
  //If it's AJAX call
  if(isset($form_state['input']['path-alias'])) {
    $translit_path = seminars_form_get_translit_path($form_state['input']['title']);
    $form['path-alias']['#default_value'] = $translit_path;
    $form_state['input']['path-alias'] = $translit_path;
    } 
     
  ##############################
  # Periods fiedsets  
  $form['periods'] = array(
    '#tree' => TRUE,
    '#type' => 'fieldset',
    '#title' => t('Seminar period(s)'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#prefix' => '<div id="periods-ajax-wrapper">',
    '#suffix' => '</div>',
    );

  if($periods_count) {
    for($i=0; $i < $periods_count; $i++) {
      $period = huf_issetor($seminars->seminars_periods[$i], NULL);
      seminars_form_periods_set($form, $form_state, $period, $i);
      }
    $form['periods']['#description'] = t('Start date - End date');
    } else {
    $form['periods']['#description'] = 
      t('No seminar periods. Click button below to add.');
    }
    
  $form['periods']['count'] = array(
    '#type' => 'hidden',
    '#value' => $periods_count,
    );

    
  $form['periods-add-set'] = array(
    '#type' => 'button',
    '#name' => 'append-period', 
    '#value' => t('+ Add a period'),
  	'#validate' => array(),
  	'#limit_validation_errors' => array(),
    '#prefix' => '<div style="float: left;">',
    '#suffix' => '</div>',
    '#ajax' => array(
      'callback' => 'seminars_form_ajax_append_period',
      'wrapper' => 'periods-ajax-wrapper',
      ),
    );
  $form['periods-delete-selected'] = array(
    '#type' => 'button',
    '#value' => t('- Delete selected periods'),
    '#executes_submit_callback' => TRUE,
    '#submit' => array('seminars_form_delete_periods'),
    );
  
  ####################################
  # Categories fieldsets
  $form['categories'] = array(
    '#type' => 'fieldset',
    '#tree' => TRUE,
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#title' => t('Seminar categories'),
    '#prefix' => '<div id="categories-ajax-wrapper">',
    '#suffix' => '</div>',
    );
  
  $form['categories']['count'] = array(
    '#type' => 'hidden',
    '#value' => $categories_count,
    '#attributes' => array(
      'id' => 'categories-count',
      ),
    );
    
  if($categories_count) {
    for($i=0; $i < $categories_count; $i++) {
    
      $category = huf_issetor($seminars->seminars_categories[$i], NULL);
      seminars_form_categories_set($form, $form_state, $category, $i);
      
      }
    }
      
  if($form['categories']['count']['#value'] == 0) {
    $form['categories']['#description'] = 
      t('No categories found. Click one of the buttons below to add.');
    }
  
  $form['categories-add-set'] = array(
    '#type' => 'button',
    '#name' => 'append-category',
    '#value' => t('+ Add a category'),
    '#validate' => array(),
    '#limit_validation_errors' => array(),
    '#ajax' => array(
      'callback' => 'seminars_form_ajax_append_category',
      'wrapper' => 'categories-ajax-wrapper',
      ),
    );
  $form['categories-remove-selected'] = array(
    '#type' => 'button',
    '#value' => t('- Remove selected categories'),
    '#executes_submit_callback' => TRUE,
    '#submit' => array('seminars_form_remove_categories'),
    );
  

  ############################  
  #       Masters fieldset
  ##############################
  
  $form['masters'] = array(
    '#type' => 'fieldset',
    '#tree' => TRUE,
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#title' => t('Seminar master(s)'),
    '#prefix' => '<div id="masters-ajax-wrapper">',
    '#suffix' => '</div>',
    );
    
  if($masters_count) {
  
    for($i=0;$i < $masters_count;$i++) {
    
      $master = huf_issetor($seminars->seminars_masters[$i], NULL);
      seminars_form_masters_set($form, $form_state, $master, $i);
      
      }
    }
      
  if(!$masters_count){
    $form['masters']['#description'] = 
      t('No masters found. Click one of the buttons below to add.');
    }
      
  $form['masters']['count'] = array(
    '#type' => 'hidden',
    '#value' => $masters_count,
    '#attributes' => array(
      'id' => 'masters-count',
      ),
    );
    
  $form['masters-add-set'] = array(
    '#type' => 'button',
    '#name' => 'append-master',
    '#value' => t('+ Add master'),
    '#validate' => array(),
    '#limit_validation_errors' => array(),
    '#ajax' => array(
      'wrapper' => 'masters-ajax-wrapper',
      'callback' => 'seminars_form_ajax_append_master',
      ),
    );
  $form['masters-remove-selected'] = array(
    '#type' => 'button',
    '#value' => t('- Remove selected masters'),
    '#executes_submit_callback' => TRUE,
    '#submit' => array('seminars_form_remove_masters'),
    );
    
  ################################
  # Seminar purpose
  $form['purpose'] = array(
    '#title' => t('Seminar purposes'),
    '#description' => t('Plain text only.'),
    '#type' => 'textarea',
    '#default_value' => check_plain(huf_issetor($seminars->purpose, NULL)),
    '#required' => FALSE,
    );
  
  #################################
  # Seminar content
  if(module_exists('ckeditor')) {
    $form['content'] = array(
      '#type' => 'text_format',
      '#base_type' => 'textarea',
      '#format' => huf_issetor($seminars->content_format, SEMINARS_SEMINARS_CONTENT_FORMAT_DEFAULT),
      '#title' => t('Seminar content'),
      '#default_value' => huf_issetor($seminars->content, NULL),
      //'#description' => t('Filtered HTML'),
      );
    } else {
    $form['content'] = array(
      '#type' => 'textarea',
      '#title' => t('Seminar content'),
      '#default_value' => huf_issetor($seminars->content, NULL),
      '#description' => t('Filtered HTML'),
      );
    }
  #################################
  # Seminar diplomas
  $form['diplomas'] = array(
    '#type' => 'textarea',
    '#title' => t('Participants recieve'),
    '#default_value' => huf_issetor($seminars->diplomas, NULL),
    '#description' => t('Filtered HTML'),
    );
  
  ##################################
  # Other fieldset
  $form['other'] = array(
    '#type' => 'fieldset',
    '#title' => t('Other'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    );
  $form['other']['created'] = array(
    '#prefix' => '<p>',
    '#suffix' => '</p>',
    '#markup' => t('Created date @date', 
      array(
        '@date' => date($date_format, 
          huf_issetor($seminars->created, REQUEST_TIME))
        )
      ),
    );
  $form['other']['changed'] = array(
    '#prefix' => '<p>',
    '#suffix' => '</p>',
    '#markup' => t('Changed date @date', 
      array(
        '@date' => date($date_format, 
          huf_issetor($seminars->created, REQUEST_TIME))
        )
      ),
    );
    
  $form['options'] = array(
    '#type' => 'fieldset',
    '#access' => seminars_access('administer nodes'),
    '#title' => t('Publishing options'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'additional_settings',
    '#attributes' => array(
      'class' => array('node-form-options'),
    ),
    '#weight' => 95,
  );
  $form['options']['status'] = array(
    '#type' => 'checkbox',
    '#title' => t('Published'),
    '#default_value' => huf_issetor($seminars->status, 0),
  );
  $form['options']['promote'] = array(
    '#type' => 'checkbox',
    '#title' => t('Promoted to front page'),
    '#default_value' => huf_issetor($seminars->promote, 0),
  );
  $form['options']['sticky'] = array(
    '#type' => 'checkbox',
    '#title' => t('Sticky at top of lists'),
    '#default_value' => huf_issetor($seminars->sticky, 0),
  );

  // Add the buttons.
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#weight' => 5,
    '#submit' => array('seminars_form_submit'),
    );
  $form['#submit'] = array('seminars_form_submit');
  /**
  $form['actions']['preview'] = array(
    '#type' => 'submit',
    '#value' => t('Preview'),
    '#weight' => 10,
    '#submit' => array('seminars_form_build_preview'),
    );**/
  
  if (!empty($seminars->sid) && seminars_access('delete', $seminars)) {
    $form['actions']['delete'] = array(
      '#markup' => l(t('Delete'), 'seminars/'.$seminars->sid.'/delete'),
      '#weight' => 15,
      );
    }
  
  $form['#attached']['library'][] = array('system', 'drupal.ajax');
  $form['#attached']['js'][] = 
    base_path().drupal_get_path('module', 'seminars').'/seminars.js';
    
  $form['#validate'] = array('seminars_form_validate');
  field_attach_form('seminars', $seminars, $form, $form_state);  
    
  return $form;  
}

function seminars_form_get_data($form_state) {
  global $user;
  static $allowed_tags;
  if(empty($allowed_tags)) 
    $allowed_tags = array('a', 'em', 'strong', 'cite', 'blockquote', 'code', 
      'ul', 'ol', 'li', 'dl', 'dt', 'dd', 
      'h1', 'h2', 'h3', 'h4', 'h5', 'br', 'div', 'p', 
      'span', 'table', 'th', 'tr', 'td', 'img',
      'object', 'param', 'embed',
      );

  $seminars = (object)array(
    'sid' => huf_issetor($form_state['values']['sid'], 0),
    'title' => check_plain(strip_tags($form_state['values']['title'])),
    
    'path_alias' => check_plain(strip_tags($form_state['values']['path-alias'])),
    'pid' => huf_issetor($form_state['values']['pid']),
  
    'purpose' => check_plain(strip_tags($form_state['values']['purpose'])),
    'diplomas' => filter_xss($form_state['values']['diplomas'], $allowed_tags),
    
    'sticky' => $form_state['values']['sticky'] ? 1 : 0,
    'promote' => $form_state['values']['promote'] ? 1 : 0, 
    'status' => $form_state['values']['status'] ? 1 : 0,
    'uid' => huf_issetor($form_state['values']['uid'], $user->uid), 
    );
  
  if(is_array($form_state['values']['content'])) {
    $seminars->content_format = $form_state['values']['content']['format'];
    switch($seminars->content_format) {
      case 'plain_text':
        $seminars->content 
          = check_plain($form_state['values']['content']['value']);
        break;
      case 'filtered_html':
        $seminars->content 
          = filter_xss($form_state['values']['content']['value']);
        break;
      case 'full_html':
      case 'free_html':
        $seminars->content 
          = filter_xss($form_state['values']['content']['value'], $allowed_tags);
        break;
      }
    } else {
    $seminars->content 
      = filter_xss($form_state['values']['content']['value'], $allowed_tags);
    $seminars->content_format = SEMINARS_SEMINARS_CONTENT_FORMAT_DEFAULT;
    }
  
  $seminars->seminars_categories = seminars_form_get_data_categories($form_state['values']);
  $seminars->seminars_masters = seminars_form_get_data_masters($form_state['values'], $allowed_tags);
  $seminars->seminars_periods = seminars_form_get_data_periods($form_state['values']);
  
  return $seminars;
  
} 

/**
 * Function get categories data from form state arrays
 * @param $form_values - form state values $form_state['values'] or $form_state['input']
 * @return array of categories seminars entity type
 **/   
function seminars_form_get_data_categories($form_values) {
  
  $seminars_categories = array();
    
  if(!empty($form_values['categories'])) {
  
    foreach($form_values['categories'] as $category) {
    
      if(is_array($category)) {
        
        if ($category['scid'] == 0 && $category['title-select'] != 0)
          $category['scid'] = $category['title-select'];
        
        $seminars_categories[] = (object)array(
        
          'scid' => huf_issetor($category['scid'], 0),
          'sid' => huf_issetor($form_values['sid'], 0),
          'title' => check_plain(strip_tags(huf_issetor($category['title'], ''))),
          'path_alias' => check_plain(strip_tags(check_plain(strip_tags($category['path-alias'])))),
          'pid' => huf_issetor($category['pid']),
          
          'description' => check_plain(strip_tags(huf_issetor($category['description'], ''))),
          
          );
        }
      }
    }
    
  return $seminars_categories;
}

/**
 * Function get masters data from form state arrays
 * @param $form_values - form state values $form_state['values'] or $form_state['input']
 * @param $allowed_tags - allowed html tags for filter_xss() func. 
 * @return array of masters seminars entity type
 **/   
function seminars_form_get_data_masters($form_values, $allowed_tags) {

  global $user;
  $seminars_masters = array();
  
  if(!empty($form_values['masters'])) {
    
    foreach($form_values['masters'] as $master) {
    
      if(is_array($master)) {
      
        if($master['smid'] == 0 && $master['fio-select'] != 0) 
          $master['smid'] = $master['fio-select'];
          
        //Save permanent photo file
        if($photo_fid = huf_issetor($master['photo_fid'], 0)) {
          seminars_photofile_permanent($photo_fid, 
            huf_issetor($form_values['uid'], $user->uid));
          }  
      
        $seminars_masters[] = (object)array(
          'smid' => huf_issetor($master['smid'], 0),
          'sid' => huf_issetor($form_values['sid'], 0),
          'fio' => filter_xss(huf_issetor($master['fio'], ''), $allowed_tags),
          'photo_fid' => $photo_fid,
          'degrees' => check_plain(strip_tags(huf_issetor($master['degrees'], ''))),
          );
        
        }
      }
    }
  
  return $seminars_masters;
} 

/**
 * Function get periods data from form state arrays
 * @param $form_values - form state values $form_state['values'] or $form_state['input']
 * @return array of periods seminars entity type
 **/   
function seminars_form_get_data_periods($form_values) {
  
  $seminars_periods = array();
  
  if(!empty($form_values['periods'])) {
  
    foreach($form_values['periods'] as $period) {
    
      if(is_array($period)) {
      
        $sd = huf_issetor($period['start-date'], FALSE);
        $ed = huf_issetor($period['end-date'], FALSE);
        
        $seminars_periods[] = (object)array(
          'spid' => huf_issetor($period['spid'], 0),
          'sid' => huf_issetor($form_values['sid'], 0),
          'start_date' => 
            ($sd ? strtotime($sd['day'].'-'.$sd['month'].'-'.$sd['year']) : 0),
          'end_date' =>
            ($ed ? strtotime($ed['day'].'-'.$ed['month'].'-'.$ed['year']) : 0), 
          );
        }
      }
    }
    
  return $seminars_periods;
}

function seminars_form_module_js() {

  $seminars_js = drupal_get_path('module', 'seminars').'/seminars.js';
  $options = array(
    'type' => 'file',
    'group' => JS_DEFAULT,
    'cache' => TRUE,
    'preprocess' => TRUE,
    );
  $js = drupal_add_js($seminars_js,$options);
  
}

function seminars_form_validate($form, &$form_state) {

  $seminars = seminars_form_get_data($form_state);
  
  if(empty($seminars)) {
    form_set_error(NULL, t('Error occured during process form data'));
    return;
    }
    
  if(empty($seminars->title)) {
    form_set_error('title', t('Seminar title required'));
    }

  field_attach_form_validate('seminars', $seminars, $form, $form_state);
  
}

function seminars_form_submit($form, &$form_state) {

  $seminars = seminars_form_get_data($form_state);
  
  field_attach_submit('seminars', $seminars, $form, $form_state);
  
  $succ = entity_get_controller('seminars')->save($seminars);
  if($succ) {
    $seminars = $succ;
    drupal_set_message(t('Seminar saved'));
    } else {
    drupal_set_message(t('Seminar not saved'), 'error');
    }
}

function seminars_form_alter(&$form, &$form_state, $form_id) {
  if($form_id == 'webform_client_form_'.SEMINARS_WEBFORM_REQUEST_FORM_ID) {
    $elements = $form['submitted'];
    
    unset($elements['request_form_date']['#needs_validation']);
    unset($elements['request_form_date']['#webform_validated']);
    
    $form['submitted'] = $elements;
    }
}

function seminars_form_build_preview() {
  //TODO: build preview page
}

/**
 * Delete seminar confirmation form
 * menu callback function
 */
function seminars_form_delete_confirm($form, &$form_state, $seminars = NULL) {

  if(!seminars_access('delete')) {
    watchdog('seminars', t('Access denied: @page'), 
      array('@page' => drupal_get_destination()), WATCHDOG_WARNING);
    $msg = t('403 Access Denied');
    drupal_set_title($msg);
    return $msg;
    }
    
  $seminars = !empty($seminars) ? $seminars : seminars_load($form_state['values']['sid']);
  
  $form['#seminars'] = $seminars;
  // Always provide entity id in the same form key as in the entity edit form.
  $form['sid'] = array('#type' => 'value', '#value' => $seminars->sid);
  return confirm_form($form,
    t('Are you sure you want to delete %title?', 
      array('%title' => check_plain($seminars->title))),
    'node/edit/seminars/' . check_plain($seminars->sid),
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
    );
}

/**
 * Delete seminars confirm submit
 */
function seminars_form_delete_confirm_submit($form, &$form_state) {

  if($form_state['values']['confirm']) {
    $seminars = seminars_load($form_state['values']['sid']);
    
    if(!seminars_access('delete')) {
      watchdog('seminars', t('Access denied: @page'), 
        array('@page' => drupal_get_destination()), WATCHDOG_WARNING);
      $msg = t('403 Access Denied');
      drupal_set_title($msg);
      return $msg;
      }

    seminars_delete($form_state['values']['sid']);
    watchdog('seminars', 
      t('Deleted @title'), array('@title' => check_plain($seminars->title)));
    drupal_set_message(t('Seminar has been deleted.'));
    }
  $form_state['redirect'] = 'admin/content';
} 

/**
 * Delete selected seminar periods
 * menu callback function
 */
function seminars_form_delete_periods($form, &$form_state) {

  if(!seminars_access('delete')) {
    watchdog('seminars', t('Access denied: @page'), 
      array('@page' => drupal_get_destination()), WATCHDOG_WARNING);
    $msg = t('403 Access Denied');
    drupal_set_title($msg);
    return $msg;
    }
    
  $seminars = !empty($seminars) ? $seminars : seminars_load($form_state['values']['sid']);
  $del_count = 0;
  foreach($form_state['values']['periods'] as $period) {
    if(isset($period['selected']) && $period['selected'] == 1) {
      if(seminars_delete_period($seminars, $period['spid'])) {
        $del_count++;
        }
      }
    }
  
  if($del_count) {  
    drupal_set_message(t('@count periods deleted from seminar "@title"', 
      array(
        '@count' => $del_count,
        '@title' => $seminars->title,
        )));
    } else {
    drupal_set_message(t('No selected periods to delete or no periods found'));
    }
    
}

/**
 * Remove selected seminar categories from seminar 
 * menu callback function
 */
function seminars_form_remove_categories($form, &$form_state) {

  if(!seminars_access('delete')) {
    watchdog('seminars', t('Access denied: @page'), 
      array('@page' => drupal_get_destination()), WATCHDOG_WARNING);
    $msg = t('403 Access Denied');
    drupal_set_title($msg);
    return $msg;
    }
    
  $seminars = !empty($seminars) ? $seminars : seminars_load($form_state['values']['sid']);
  $del_count = 0;
  foreach($form_state['values']['categories'] as $category) {
    if(isset($category['selected']) && $category['selected'] == 1) {
      if(seminars_remove_category($seminars, $category['scid'])) {
        $del_count++;
        }
      }
    }
  
  if($del_count) {  
    drupal_set_message(t('@count categories removed from seminar "@title"', 
      array(
        '@count' => $del_count,
        '@title' => $seminars->title,
        )));
    } else {
    drupal_set_message(t('No selected categories to remove or no categories found'));
    }
    
}

/**
 * Remove selected seminar masters from seminar 
 * menu callback function
 */
function seminars_form_remove_masters($form, &$form_state) {

  if(!seminars_access('delete')) {
    watchdog('seminars', t('Access denied: @page'), 
      array('@page' => drupal_get_destination()), WATCHDOG_WARNING);
    $msg = t('403 Access Denied');
    drupal_set_title($msg);
    return $msg;
    }
    
  $seminars = !empty($seminars) ? $seminars : seminars_load($form_state['values']['sid']);
  $del_count = 0;
  foreach($form_state['values']['masters'] as $master) {
    if(isset($master['selected']) && $master['selected'] == 1) {
      if(seminars_remove_master($seminars, $master['smid'])) {
        $del_count++;
        }
      }
    }
  
  if($del_count) {  
    drupal_set_message(t('@count masters removed from seminar "@title"', 
      array(
        '@count' => $del_count,
        '@title' => $seminars->title,
        )));
    } else {
    drupal_set_message(t('No selected masters to remove or no masters found'));
    }
    
}

/**
 * Function return translit of title, 
 *  all spaces replaced with '-'
 *  all 'unknown' symbols skiped 
 **/ 
function seminars_form_get_translit_path($title, $unique = FALSE) {
  if(isset($title) 
    && !empty($title) 
    && function_exists('transliteration_get')
    ){
    $translit = strip_tags($title);
    $translit = str_replace(' ', '-', $translit);
    $translit = preg_replace("/--+/", '-', $translit);
    $translit = transliteration_get($translit, '', language_default('language'));
    $translit = mb_strtolower($translit);
    $translit = preg_replace("/[^a-z-]+/", '', $translit);
    
    //Check unique
    if($unique) {
      $count = 0;
      $clean = $translit;
      while(path_load(array('alias' => $translit))) {
        $translit = $clean.'-'.$count;
        $count++;
        }
      }
      
    } else { 
    $translit = NULL;
    }
  return $translit;
}
    
/**
 *
 *           AJAX functions
 *  
 **/ 

/**
 * AJAX parse trigger element
 * @param $element - ajax trigger element at $form_state['triggering_element']
 * @return object with ajax trigger info
 **/  
function seminars_form_ajax_get_trigger($element){

  #Triggering AJAX
  $trigger = (object)array(
    'ajax' => !empty($element['#name']),
    'action' => NULL,
    'set' => NULL,
    'param' => NULL,
    'fsid' => NULL,
    ); 
    
  if($trigger->ajax) {
  
    $trig = explode('-', $element['#name']);
    
    if(count($trig) == 2) {
    
      list($trigger->action, $trigger->set) = $trig;
      
      } elseif(count($trig) == 4) {
      
      list(
        $trigger->action, 
        $trigger->set, 
        $trigger->param,
        $trigger->id) = $trig;
      
      $trigger->fsid = implode('-', array($trigger->set, $trigger->param));
      
      }
    }
    
  return $trigger;
}

/**
 * AJAX callback for seminar path alias
 **/
function seminars_form_ajax_seminar_path_alias($form, &$form_state) {
  return $form['path-alias'];
} 

/**
 * AJAX add category set
 * @param $form - array of form that call AJAX
 * @param $form_state - ref to $form_state array
 * @return - new categories wrapper fieldset    
 **/  
function seminars_form_ajax_append_category($form, &$form_state) {
  return $form['categories'];
}

/**
 *  AJAX translit path alias
 **/ 
function seminars_form_ajax_category_path_alias($form, &$form_state) {
  $fsid = $form_state['input']['_triggering_element_name'];
  #$form_state['input']['categories'][$fsid]['path-alias'] 
  #  = $form['categories'][$fsid]['path-alias']['#default_value'];
  //DEBUG:
  //watchdog('seminars', 'AJAX path-alias = '.dvr($form['categories'][$fsid]['path-alias'], TRUE));
  return $form['categories'][$fsid]['path-alias'];
}

/**
 * AJAX add master fieldset to wrapper
 * @param $form - entire form array
 * @param &$form_state - state of entire form
 * @return fieldset
 **/    
function seminars_form_ajax_append_master($form, &$form_state) {
  return $form['masters'];
}

/**
 * AJAX callback for add period to seminar
 **/ 
function seminars_form_ajax_append_period($form, &$form_state) {
  return $form['periods'];
}

/**
 * Empty ajax validate function
 **/ 
function seminars_ajax_validate($form, &$form_state) {
  #kpr($form);
  #print_r($form_state['values']);
}

function seminars_ajax_master_data() {

  if(!empty($_POST)) {
    
    $master = seminars_load_master($_POST['smid']);
                                       
    if($master) {
      if(is_array($master)) $master = array_shift($master);
      
      $commands = array();
      
      $commands[] = 
        ajax_command_data(
          '#masters-'.$_POST['fsid'].'-smid',
          'value',
          $master->smid
          );

      $commands[] = 
        ajax_command_data(
          '#edit-masters-'.$_POST['fsid'].'-fio',
          'value',
          $master->fio
          );
      
      $commands[] = 
        ajax_command_data(
          '#edit-masters-'.$_POST['fsid'].'-degrees',
          'value',
          $master->degrees
          );

      $commands[] = 
        ajax_command_data(
          '#edit-masters-'.$_POST['fsid'].'-photo-fid-upload',
          'value',
          $master->photo_fid
          );
          
      $commands[] = 
        ajax_command_data(
          '#masters-'.$_POST['fsid'].'-photo',
          'src',
          seminars_get_master_photo($master->photo_fid)
          );
          
      ajax_deliver(array('#type' => 'ajax', '#commands' => $commands));
      } else {
      ajax_deliver(
        array(
          '#type' => 'ajax', 
          '#commands' => array(
            ajax_command_alert(t('Master not found'))
            )
          )
        );
      }
      
    }
}

/**
 * AJAX callback
 * send transliteration of received title
 **/
/**  
function seminars_ajax_translit_path() {

  if(!empty($_POST)) {
    if(isset($_POST['title']) && function_exists('transliteration_get')) {
      $translit_path = mb_strtolower(transliteration_get(
        preg_replace("/--+/", '-', str_replace(' ', '-', $_POST['title'])), '', language_default('language')
        ));
      } else { 
      $translit_path = NULL;
      }
      
    $commands = array();
    
    $commands[] = ajax_command_data(
      'translit_path',
      'value',
      $translit_path
      );
      
    ajax_deliver(array('#type' => 'ajax', '#commands' => $commands));
    }
}  **/

function seminars_ajax_category_data() {

  if(!empty($_POST)) {
    
    $category = seminars_load_category($_POST['scid']);
                                       
   
    if($category) {
      if(is_array($category)) $category = array_shift($category);
      
      $commands = array();
      
      $commands[] = 
        ajax_command_data(
          '#categories-'.$_POST['fsid'].'-scid',
          'value',
          $category->scid
          );
      
      $commands[] = 
        ajax_command_data(
          '#edit-categories-'.$_POST['fsid'].'-title',
          'value',
          $category->title
          );
      
      $commands[] = 
        ajax_command_data(
          '#edit-categories-'.$_POST['fsid'].'-description',
          'value',
          $category->description
          );

      ajax_deliver(array('#type' => 'ajax', '#commands' => $commands));
      } else {
      ajax_deliver(
        array(
          '#type' => 'ajax', 
          '#commands' => array(
            ajax_command_alert(t('Category not found'))
            )
          )
        );
      }
      
    }
}

function seminars_request_form($form, &$form_state, $seminars) {
  
  if(module_exists('webform')) {
    $form = node_view(node_load(SEMINARS_WEBFORM_REQUEST_FORM_ID));
    $elements = $form['webform']['#form']['submitted'];
  
    //DEBUG:
    #$form['debug_seminars']['#markup'] = kpr($seminars, TRUE);
    #$form['debug_origin']['#markup'] = kpr($form, TRUE);
    #$form['debug_elements_o']['#markup'] = kpr($elements, TRUE);
    
    $elements['request_form_seminar']['#markup'] 
      .= '<span>'.$seminars->title.'</span>';
    
    if(count($seminars->seminars_periods)) {  
      unset($elements['request_form_date']['#options']);
      foreach($seminars->seminars_periods as $period) {
        $dates = date('d-m-Y', $period->start_date)
          .' - '.
          date('d-m-Y', $period->end_date);
        #$elements['request_form_date']['#options'][$period->spid] = $dates;
        $elements['request_form_date']['#options'][$dates] = $dates;
        }
      } else {
      unset($elements['request_form_date']);
      }
      
    //DEBUG:
    #$form['debug_elements']['#markup'] = kpr($elements, TRUE);
    
    $form['webform']['#form']['submitted'] = $elements;
    
    } else {
    $form = array();
    drupal_set_message(
      t('Required Webform module and created request form. Contact to your site administrator.'),
      'error'
      ); 
    }
    
  return $form;
}

/**
 *  Function return array of {seminars_masters}.FIO
 *  @param - no parameters
 *  @return - array('smid_1' =>'FIO 1', 'smid_2' =>'FIO 2', ...)
 **/
function seminars_masters_get_FIO() {

  $que = db_select('seminars_masters', 'sm')->fields('sm', array('smid','FIO'));
  $que->orderBy('sm.FIO');
  $res = $que->execute()->fetchAll();
  $masters = array();
  foreach($res as $rec) {
    $masters[$rec->smid] = $rec->FIO;
    } 
  return $masters;
}      

/**
 * Function makes loaded file as 'permanent'
 * @param $fid - file id
 * @param $account_id - account id
 * @return - no return
 **/    
function seminars_photofile_permanent($fid, $account_id = NULL) {

  global $user;
  if(!isset($account_id)) {
    $account_id = $user->uid;
    }
  
  // Load the file via file.fid.
  $file = file_load($fid);
  // Change status to permanent.
  $file->status = FILE_STATUS_PERMANENT;
  // Save.
  file_save($file);
  // Record that the module (in this example, user module) is using the file. 
  file_usage_add($file, 'seminars', 'seminars', $account_id); 
   
}

/**
 *  Function return array of {seminars_categories}.titles
 *  @param - no parameters
 *  @return - array('scid_1' => 'title 1', 'scid 2' => 'title 2', ...)
 */
function seminars_categories_get_titles($get_empty = TRUE, $get_unpublished = TRUE) {

  $que = db_select('seminars_categories', 'sc');
  $que->fields('sc', array('scid', 'title'));
  $que->orderBy('sc.title');
  $res = $que->execute()->fetchAll();

  $categories = array();
  foreach($res as $rec) {
  
    if(!$get_empty) {
      $que_notempty = db_query('SELECT COUNT(*) FROM {seminars_categories_relations}'
        .' WHERE scid = :scid', array(':scid' => $rec->scid));
      
      $count = $que_notempty->fetchColumn();
      
      if(!$count) continue;
      }
    
    if(!$get_unpublished) {
      $que_published = db_query('SELECT COUNT(*) FROM {seminars} AS s'
        .' WHERE s.sid IN '
        .'(SELECT scr.sid FROM {seminars_categories_relations} AS scr '
        .' WHERE scr.scid = :scid)'
        .' AND s.status = 1', array(':scid' => $rec->scid));
      
      $count = $que_published->fetchColumn();
      
      if(!$count) continue;
      }

    $categories[$rec->scid] = $rec->title;
    }
    
  return $categories; 
} 

