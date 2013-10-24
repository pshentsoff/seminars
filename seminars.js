// Seminars module javascript functions
// @author Vadim Pshentsov <pshentsoff@yandex.ru>

function seminarsMastersFIOSelectChange(fsid) {
  var fio_select = jQuery('#edit-masters-'+fsid+'-fio-select'); 
  if(fio_select.val() != 0) {
    //jQuery('#masters-'+fsid+'-smid').val(fio_select.val());
    jQuery.post(
      Drupal.settings.basePath+'seminars/ajax/master/data',
      {
        smid: fio_select.val(),
        fsid: fsid
        },
        function(data) {
          if(data[1].command == 'alert') {
            alert(data[1].text);
            return false;
            }
          if(data[0].command == 'settings') {
            for(var i = 1; i < data.length; i++) {
              jQuery(data[i].selector).attr(data[i].name, data[i].value);
              }
            }
          }
      );
      
    }
  return true;
}

function seminarsMastersFIOTextChange(fsid) {
  //jQuery('#masters-'+fsid+'-smid').val(0);
  //jQuery('#edit-masters-'+fsid+'-fio-select').val(0);
  return true;
}

function seminarsCategoriesTitleSelectChange(fsid) {
  var title_select = jQuery('#edit-categories-'+fsid+'-title-select'); 
  //jQuery('#categories-'+fsid+'-scid').val(title_select.val());
  if(title_select.val() != 0) {
    jQuery.post(
      Drupal.settings.basePath+'seminars/ajax/category/data',
      {
        scid: title_select.val(),
        fsid: fsid
        },
        function(data) {
          if(data[1].command == 'alert') {
            alert(data[1].text);
            return false;
            }
          if(data[0].command == 'settings') {
            for(var i=1; i<data.length; i++) {
              jQuery(data[i].selector).val(data[i].value);
              }
            }
          }
      );
      
    }
  return true;
}

function seminarsCategoriesTitleTextChange(fsid) {
  //jQuery('#categories-'+fsid+'-scid').val(0);
  //jQuery('#edit-categories-'+fsid+'-title-select').val(0);
  return true;
}
