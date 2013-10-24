<?php

/**
  * @file
  * Seminars admin functions include  
  *
  * @author Vadim Pshentsov <pshentsoff@yandex.ru>
  *   
  */  

function seminars_admin_content($form, $form_state) {
  if(!function_exists('node_admin_nodes')) {
    module_load_include('inc', 'node', 'node.admin');
    }

  if (isset($form_state['values']['operation']) && $form_state['values']['operation'] == 'delete') {
    return node_multiple_delete_confirm($form, $form_state, array_filter($form_state['values']['nodes']));
  }
  
  $form['filter'] = node_filter_form();
  $form['#submit'][] = 'node_filter_form_submit';
  $form['admin'] = node_admin_nodes();
  $form['admin_seminars'] = seminars_admin_seminars();

  return $form;
}

function seminars_admin_seminars() {

  $admin_access = user_access('create, edit, delete seminars');

  // Build the 'Update options' form.
  $form['options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Seminars').': '.t('Update options'),
    '#attributes' => array('class' => array('container-inline')),
    '#access' => $admin_access,
    );
  $options = array();
  $operations = module_invoke_all('seminars_operations');
  foreach ($operations as $operation => $array) {
    $options[$operation] = $array['label'];
    
    }
  
  $form['options']['operation'] = array(
    '#type' => 'select',
    '#title' => t('Operation'),
    '#title_display' => 'invisible',
    '#options' => $options,
    '#default_value' => 'approve',
    );
  $form['options']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update'),
    '#validate' => array('seminars_admin_seminars_validate'),
    '#submit' => array('seminars_admin_seminars_submit'),
    );
  
  // Build the sortable table header.
  $header = array(
    'title' => array('data' => t('Title'), 'field' => 'n.title'),
    'type' => t('Type'),
    'author' => t('Author'),
    'status' => array('data' => t('Status'), 'field' => 'n.status'),
    'changed' => array('data' => t('Updated'), 'field' => 'n.changed', 'sort' => 'desc')
    );
  
  $header['operations'] = array('data' => t('Operations'));
  
  $query = db_select('seminars', 'n')->extend('PagerDefault')->extend('TableSort');
  #seminars_build_filter_query($query);
  
  $sids = $query
    ->fields('n',array('sid'))
    ->limit(50)
    ->orderByHeader($header)
    ->execute()
    ->fetchCol();
  $seminars = seminars_load_multiple($sids);
  
  $destination = drupal_get_destination();
  $options = array();
  
  foreach ($seminars as $seminar) {
  
    $account = user_load($seminar->uid);
    
    $options[$seminar->sid] = array(
      'title' => array(
        'data' => array(
          '#type' => 'link',
          '#title' => $seminar->title,
          '#href' => 'seminars/' . $seminar->sid,
          '#options' => array(),
          '#suffix' => ' ' . theme('mark', array('type' => node_mark($seminar->sid, $seminar->changed))),
          ),
        ),
      'type' => t('Seminars'),
      'author' => theme('username', array('account' => $account)),
      'status' => $seminar->status ? t('published') : t('not published'),
      'changed' => format_date($seminar->changed, 'short'),
      );
    
    // Build a list of all the accessible operations
    $operations = array();
    if (seminars_access('edit')) {
      $operations['edit'] = array(
        'title' => t('edit'),
        'href' => 'seminars/' . $seminar->sid . '/edit',
        'query' => $destination,
      );
    }
    
    if (seminars_access('delete')) {
      $operations['delete'] = array(
        'title' => t('delete'),
        'href' => 'seminars/' . $seminar->sid . '/delete',
        'query' => $destination,
      );
    }
    $options[$seminar->sid]['operations'] = array();
    if (count($operations) > 1) {
      // Render an unordered list of operations links.
      $options[$seminar->sid]['operations'] = array(
        'data' => array(
          '#theme' => 'links__node_operations',
          '#links' => $operations,
          '#attributes' => array('class' => array('links', 'inline')),
          ),
        );
      } elseif (!empty($operations)) {
      // Render the first and only operation as a link.
      $link = reset($operations);
      $options[$seminar->sid]['operations'] = array(
        'data' => array(
          '#type' => 'link',
          '#title' => $link['title'],
          '#href' => $link['href'],
          '#options' => array('query' => $link['query']),
          ),
        );
      }
    }
  
  if ($admin_access) {
    $form['seminars'] = array(
      '#type' => 'tableselect',
      '#header' => $header,
      '#options' => $options,
      '#empty' => t('No content available.'),
      );
    } else {
    $form['seminars'] = array(
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => $options,
      '#empty' => t('No content available.'),
      );
    }
  
  $form['pager'] = array('#markup' => theme('pager'));
  return $form;
}

function seminars_admin_seminars_validate($form, &$form_state) {
  // Error if there are no items to select.
  if(!is_array($form_state['values']['seminars']) || 
    !count(array_filter($form_state['values']['seminars']))) {
    form_set_error('', t('No items selected.'));
    }
}

function seminars_admin_seminars_submit($form, &$form_state) {

  $operations = module_invoke_all('seminars_operations');
  $operation = $operations[$form_state['values']['operation']];
  // Filter out unchecked seminars
  $seminars = array_filter($form_state['values']['seminars']);
  if ($function = $operation['callback']) {
    // Add in callback arguments if present.
    if (isset($operation['callback arguments'])) {
      $args = array_merge(array($seminars), $operation['callback arguments']);
      } else {
      $args = array($seminars);
      }
      
    call_user_func_array($function, $args);

    cache_clear_all();
    } else {
    // We need to rebuild the form to go to a second step. For example, to
    // show the confirmation form for the deletion of nodes.
    $form_state['rebuild'] = TRUE;
    }

}

function seminars_mass_update($seminars, $updates) {
  // We use batch processing to prevent timeout when updating a large number
  // of nodes.
  if (count($seminars) > 10) {
    $batch = array(
      'operations' => array(
        array('_seminars_mass_update_batch_process', array($seminars, $updates))
      ),
      'finished' => '_seminars_mass_update_batch_finished',
      'title' => t('Processing'),
      // We use a single multi-pass operation, so the default
      // 'Remaining x of y operations' message will be confusing here.
      'progress_message' => '',
      'error_message' => t('The update has encountered an error.'),
      // The operations do not live in the .module file, so we need to
      // tell the batch engine which file to load before calling them.
      'file' => drupal_get_path('module', 'seminars') . '/seminars.admin.inc',
    );
    batch_set($batch);
  }
  else {
    foreach ($seminars as $sid) {
      _seminars_mass_update_helper($sid, $updates);
    }
    drupal_set_message(t('The update has been performed.'));
  }
}

/**
 * Seminars Mass Update - helper function.
 */
function _seminars_mass_update_helper($sid, $updates) {

  $seminar = seminars_load($sid, NULL, TRUE);
  
  foreach ($updates as $name => $value) {
    $seminar->$name = $value;
  }
  
  seminars_save($seminar);
  
  return $seminar;
}

/**
 * Seminars Mass Update Batch operation
 */
function _seminars_mass_update_batch_process($seminars, $updates, &$context) {
  if (!isset($context['sandbox']['progress'])) {
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['max'] = count($seminars);
    $context['sandbox']['seminars'] = $seminars;
  }

  // Process seminars by groups of 5.
  $count = min(5, count($context['sandbox']['seminars']));
  for ($i = 1; $i <= $count; $i++) {
    // For each sid, load the seminar, reset the values, and save it.
    $sid = array_shift($context['sandbox']['seminars']);
    $seminar = _seminars_mass_update_helper($sid, $updates);

    // Store result for post-processing in the finished callback.
    $context['results'][] = l($seminar->title, 'seminars/' . $seminar->sid);

    // Update our progress information.
    $context['sandbox']['progress']++;
  }

  // Inform the batch engine that we are not finished,
  // and provide an estimation of the completion level we reached.
  if ($context['sandbox']['progress'] != $context['sandbox']['max']) {
    $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
  }
}

/**
 * Seminars Mass Update Batch 'finished' callback.
 */
function _seminars_mass_update_batch_finished($success, $results, $operations) {
  if ($success) {
    drupal_set_message(t('The update has been performed.'));
    } else {
    drupal_set_message(t('An error occurred and processing did not complete.'), 'error');
    $message = format_plural(count($results), '1 item successfully processed:', '@count items successfully processed:');
    $message .= theme('item_list', array('items' => $results));
    drupal_set_message($message);
    }
}

/**
 * Implements hook_node_operations().
 */
function seminars_seminars_operations() {
  $operations = array(
    'publish' => array(
      'label' => t('Publish selected content'),
      'callback' => 'seminars_mass_update',
      'callback arguments' => array('updates' => array('status' => NODE_PUBLISHED)),
    ),
    'unpublish' => array(
      'label' => t('Unpublish selected content'),
      'callback' => 'seminars_mass_update',
      'callback arguments' => array('updates' => array('status' => NODE_NOT_PUBLISHED)),
    ),
    'promote' => array(
      'label' => t('Promote selected content to front page'),
      'callback' => 'seminars_mass_update',
      'callback arguments' => array('updates' => array('status' => NODE_PUBLISHED, 'promote' => NODE_PROMOTED)),
    ),
    'demote' => array(
      'label' => t('Demote selected content from front page'),
      'callback' => 'seminars_mass_update',
      'callback arguments' => array('updates' => array('promote' => NODE_NOT_PROMOTED)),
    ),
    'sticky' => array(
      'label' => t('Make selected content sticky'),
      'callback' => 'seminars_mass_update',
      'callback arguments' => array('updates' => array('status' => NODE_PUBLISHED, 'sticky' => NODE_STICKY)),
    ),
    'unsticky' => array(
      'label' => t('Make selected content not sticky'),
      'callback' => 'seminars_mass_update',
      'callback arguments' => array('updates' => array('sticky' => NODE_NOT_STICKY)),
    ),
    /**
    'delete' => array(
      'label' => t('Delete selected content'),
      'callback' => NULL,
    ), **/
  );
  return $operations;
}
