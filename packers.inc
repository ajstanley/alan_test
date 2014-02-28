<?php

/**
 * @file
 * Islandora packers plus solution pack module
 */

/**
 * Implements hook_views_api().
 */
function packers_solution_pack_views_api() {
  return array(
    'api' => 3.0,
    'path' => drupal_get_path('module', 'packers_solution_pack') . '/includes',
  );
}

/**
 * Implements hook_islandora_breadcrumbs_alter().
 */
function packers_solution_pack_islandora_breadcrumbs_alter(&$breadcrumbs, $context, $object = NULL) {
  // Strip out html on breadcrumbs for now because they lead to RI driven
  // displays that are slow. We'll replace this with proper solr searches
  // post-demo.
  for ($i = 0; $i < count($breadcrumbs); $i++) {
    $breadcrumbs[$i] = strip_tags($breadcrumbs[$i]);
  }
}

/**
 * Implements hook_menu().
 */
function packers_solution_pack_menu() {
  $items['packers_solution_pack/pdfs_for_purchase_order/%'] = array(
    'title' => "There are no PDFs for @purchase_order_number",
    'title arguments' => array('@purchase_order_number' => 2),
    'file' => 'packers_solution_pack.inc',
    'page callback' => 'packers_solution_pack_get_pdfs_for_purchase_order',
    'page arguments' => array(2),
    'type' => MENU_CALLBACK,
    'access callback' => TRUE,
  );
  $items['packers_solution_pack/main_search'] = array(
    'page callback' => 'main_search_block_view',
    'access callback' => TRUE,
    'title' => 'Quick Search',
  );

  return $items;
}

/**
 * Implements main_search_block_info().
 */
function packers_solution_pack_block_info() {
  $blocks = array();
  $blocks['main_search_block'] = array(
    'info' => t('Quick Search'),

  );

  return $blocks;
}

/**
 * Implements main_serach_block_view().
 */
function packers_solution_pack_block_view(){
  $block['content'] = drupal_get_form('packers_solution_main_search_block_form');
  return $block;
}

/**
 * Implements hook_islandora_object_ingested().
 *
 * This function will add security to the collection objects.
 */
function packers_solution_pack_islandora_object_ingested(AbstractObject $object) {
  module_load_include('inc', 'islandora_xacml_api', 'IslandoraXacml');
  $pid = $object->id;
  $permissions = array(
    'packersPlus:MTRs' => 'access_MTRs',
    'packersPlus:MTR_PDFs' => 'access_MTR_PDFs',
    'packersPlus:birth_certificates' => 'access_birth_certificates',
    'packersPlus:assemblies' => 'access_assemblies',
    'packersPlus:part_birth_certificates' => 'access_part_birth_certificates',
    'packersPlus:engineering_items' => 'access_engineering_items',
    'packersPlus:Pressure_and_Shear_Data' => 'access_pressure_and_shear_data',
    'packersPlus:shipping_reports' => 'access_shipping_reports',
    'packersPlus:domestic_shipping_reports' => 'access_domestic_shipping_reports',
    'packersPlus:international_shipping_reports' => 'access_international_shipping_reports',
    'packersPlus:scrap_requests' => 'access_scrap_requests',
    'packersPlus:tool_conversion_reports' => 'access_tool_conversion_reports',
    'packersPlus:engineering_change_notices' => 'access_engineering_change_notices',
    'packersPlus:assembly_data_collection' => 'access_assembly_data_collection',
    'packersPlus:jobs' => 'access_jobs',
    'packersPlus:miscellaneous_job_files' => 'access_miscellaneous_job_files',
    'packersPlus:domestic_jobs' => 'access_domestic_jobs',
    'packersPlus:international_jobs' => 'access_international_jobs',
    'packersPlus:miscellaneous_jobs' => 'access_miscellaneous_jobs',
    'packersPlus:sub_assembly_drawings' => 'access_sub_assembly_drawings',
    'packersPlus:quality_assurance_collection' => 'access_quality_assurance_collection',
    'packersPlus:quality_assurance_images' => 'access_quality_assurance_images',
    'packersPlus:shift_tests' => 'access_shift_tests',
    'packersPlus:torque_tests' => 'access_torque_tests',
    'packersPlus:coordinate_measurements' => 'access_coordinate_measurements',
    'packersPlus:faros' => 'access_faros',
    'packersPlus:NCRs' => 'access_NCRs',
    'packersPlus:QC_NCRs' => 'access_QC_NCRs',
    'packersPlus:raw_material_NCRs' => 'access_raw_material_NCRs',
    'packersPlus:purchasing_NCRs' => 'access_purchasing_NCRs',
    'packersPlus:assembly_NCRs' => 'access_assembly_NCRs',
    'packersPlus:database_NCRs' => 'access_database_NCRs',
    'packersPlus:sales_documents' => 'access_sales_documents',
    'packersPlus:sales_orders' => 'access_sales_orders',
    'packersPlus:international_sales_orders' => 'access_international_sales_orders',
    'packersPlus:sales_quotes' => 'access_sales_quotes',
    'packersPlus:international_sales_quotes' => 'access_international_sales_quotes',
  );
  if (array_key_exists($pid, $permissions)) {
    $xacml = new IslandoraXacml($object);
    $xacml->viewingRule->addRole($permissions[$pid]);

    // Make sure admin retains access.
    $xacml->managementRule->addUser('admin');
    $xacml->viewingRule->addUser('admin');

    $xacml->writeBackToFedora();
  }
}

/**
 * Implements hook_islandora_required_object().
 */
function packers_solution_pack_islandora_required_objects(IslandoraTuque $connection) {
  module_load_include('inc', 'packers_solution_pack', 'packers_solution_pack');
  $module_path = drupal_get_path('module', 'packers_solution_pack');

  // Collection object definitions.
  // Birth certificates.
  $birth_certificate_collection = $connection->repository->constructObject('packersPlus:birth_certificates');
  $birth_certificate_collection->owner = 'fedoraAdmin';
  $birth_certificate_collection->label = 'Birth Certificates';
  $birth_certificate_collection->models = 'islandora:collectionCModel';
  $birth_certificate_collection->relationships->add(
    FEDORA_RELS_EXT_URI,
    'isMemberOfCollection',
    variable_get('islandora_repository_pid', 'islandora:root')
  );
  packers_solution_pack_add_tn_datastream($birth_certificate_collection);

  // Birth Certificates- Assemblies.
  $birth_certificate_assemblies_collection = $connection->repository->constructObject('packersPlus:assemblies');
  $birth_certificate_assemblies_collection->owner = 'fedoraAdmin';
  $birth_certificate_assemblies_collection->label = 'Assemblies';
  $birth_certificate_assemblies_collection->models = 'islandora:collectionCModel';
  $birth_certificate_assemblies_collection->relationships->add(
    FEDORA_RELS_EXT_URI,
    'isMemberOfCollection',
    'packersPlus:birth_certificates'
  );
  packers_solution_pack_add_tn_datastream($birth_certificate_assemblies_collection);

  // Birth Certificates- Parts.
  $birth_certificate_parts_collection = $connection->repository->constructObject('packersPlus:part_birth_certificates');
  $birth_certificate_parts_collection->owner = 'fedoraAdmin';
  $birth_certificate_parts_collection->label = 'Part Birth Certificates';
  $birth_certificate_parts_collection->models = 'islandora:collectionCModel';
  $birth_certificate_parts_collection->relationships->add(
    FEDORA_RELS_EXT_URI,
    'isMemberOfCollection',
    'packersPlus:birth_certificates'
  );
  packers_solution_pack_add_tn_datastream($birth_certificate_parts_collection);

  // Engineering Items.
  $engineering_items_collection = $connection->repository->constructObject('packersPlus:engineering_items');
  $engineering_items_collection->owner = 'fedoraAdmin';
  $engineering_items_collection->label = 'Engineering Items';
  $engineering_items_collection->models = 'islandora:collectionCModel';
  $engineering_items_collection->relationships->add(
    FEDORA_RELS_EXT_URI,
    'isMemberOfCollection',
    'packersPlus:birth_certificates'
  );
  packers_solution_pack_add_tn_datastream($engineering_items_collection);

  // Quality Assurance Collection.
  $quality_assurance_collection = $connection->repository->constructObject('packersPlus:quality_assurance_collection');
  $quality_assurance_collection->owner = 'fedoraAdmin';
  $quality_assurance_collection->label = 'Quality Assurance Collection';
  $quality_assurance_collection->models = 'islandora:collectionCModel';
  $quality_assurance_collection->relationships->add(
    FEDORA_RELS_EXT_URI,
    'isMemberOfCollection',
    variable_get('islandora_repository_pid', 'islandora:root')
  );
  packers_solution_pack_add_tn_datastream($quality_assurance_collection);

  // Pressure/shear tests.
  $peressure_and_sheer_collection = $connection->repository->constructObject('packersPlus:Pressure_and_Shear_Data');
  $peressure_and_sheer_collection->owner = 'fedoraAdmin';
  $peressure_and_sheer_collection->label = 'Pressure and Shear Data';
  $peressure_and_sheer_collection->models = 'islandora:collectionCModel';
  $peressure_and_sheer_collection->relationships->add(
    FEDORA_RELS_EXT_URI,
    'isMemberOfCollection',
    'packersPlus:quality_assurance_collection'
  );
  packers_solution_pack_add_tn_datastream($peressure_and_sheer_collection);

  // Shipping reports.
  $shipping_reports_collection = $connection->repository->constructObject('packersPlus:shipping_reports');
  $shipping_reports_collection->owner = 'fedoraAdmin';
  $shipping_reports_collection->label = 'Shipping Reports';
  $shipping_reports_collection->models = 'islandora:collectionCModel';
  $shipping_reports_collection->relationships->add(
    FEDORA_RELS_EXT_URI,
    'isMemberOfCollection',
    variable_get('islandora_repository_pid', 'islandora:root')
  );
  packers_solution_pack_add_tn_datastream($shipping_reports_collection);

  // Domestic shipping reports.
  $domestic_shippping_reports_collection = $connection->repository->constructObject('packersPlus:domestic_shipping_reports');
  $domestic_shippping_reports_collection->owner = 'fedoraAdmin';
  $domestic_shippping_reports_collection->label = 'Domestic Shipping Reports';
  $domestic_shippping_reports_collection->models = 'islandora:collectionCModel';
  $domestic_shippping_reports_collection->relationships->add(
    FEDORA_RELS_EXT_URI,
    'isMemberOfCollection',
    'packersPlus:shipping_reports'
  );
  packers_solution_pack_add_tn_datastream($domestic_shippping_reports_collection);

  // International shipping reports.
  $international_shippping_reports_collection = $connection->repository->constructObject('packersPlus:international_shipping_reports');
  $international_shippping_reports_collection->owner = 'fedoraAdmin';
  $international_shippping_reports_collection->label = 'International Shipping Reports';
  $international_shippping_reports_collection->models = 'islandora:collectionCModel';
  $international_shippping_reports_collection->relationships->add(
    FEDORA_RELS_EXT_URI,
    'isMemberOfCollection',
    'packersPlus:shipping_reports'
  );
  packers_solution_pack_add_tn_datastream($international_shippping_reports_collection);

  // Scrap Requests.
  $scrap_collection = $connection->repository->constructObject('packersPlus:scrap_requests');
  $scrap_collection->owner = 'fedoraAdmin';
  $scrap_collection->label = 'Scrap Requests';
  $scrap_collection->models = 'islandora:collectionCModel';
  $scrap_collection->relationships->add(
    FEDORA_RELS_EXT_URI,
    'isMemberOfCollection',
    variable_get('islandora_repository_pid', 'islandora:root')
  );
  packers_solution_pack_add_tn_datastream($scrap_collection);

  // Tool conversion reports.
  $tool_conversion_collection = $connection->repository->constructObject('packersPlus:tool_conversion_reports');
  $tool_conversion_collection->owner = 'fedoraAdmin';
  $tool_conversion_collection->label = 'Tool Conversion Reports';
  $tool_conversion_collection->models = 'islandora:collectionCModel';
  $tool_conversion_collection->relationships->add(
    FEDORA_RELS_EXT_URI,
    'isMemberOfCollection',
    variable_get('islandora_repository_pid', 'islandora:root')
  );
  packers_solution_pack_add_tn_datastream($tool_conversion_collection);

  // Engineering change notices.
  $engineering_change_collection = $connection->repository->constructObject('packersPlus:engineering_change_notices');
  $engineering_change_collection->owner = 'fedoraAdmin';
  $engineering_change_collection->label = 'Engineering Change Notices';
  $engineering_change_collection->models = 'islandora:collectionCModel';
  $engineering_change_collection->relationships->add(
    FEDORA_RELS_EXT_URI,
    'isMemberOfCollection',
    variable_get('islandora_repository_pid', 'islandora:root')
  );
  packers_solution_pack_add_tn_datastream($engineering_change_collection);

  // MTR.
  $mtr_collection = $connection->repository->constructObject('packersPlus:MTRs');
  $mtr_collection->owner = 'fedoraAdmin';
  $mtr_collection->label = 'MTRs';
  $mtr_collection->models = 'islandora:collectionCModel';
  $mtr_collection->relationships->add(
    FEDORA_RELS_EXT_URI,
    'isMemberOfCollection',
    variable_get('islandora_repository_pid', 'islandora:root')
  );
  packers_solution_pack_add_tn_datastream($mtr_collection);

  // MTR PDFs.
  $mtr_pdf_collection = $connection->repository->constructObject('packersPlus:MTR_PDFs');
  $mtr_pdf_collection->owner = 'fedoraAdmin';
  $mtr_pdf_collection->label = 'MTR PDFs';
  $mtr_pdf_collection->models = 'islandora:collectionCModel';
  $mtr_pdf_collection->relationships->add(
    FEDORA_RELS_EXT_URI,
    'isMemberOfCollection',
    variable_get('islandora_repository_pid', 'islandora:root')
  );
  packers_solution_pack_add_tn_datastream($mtr_pdf_collection);

  // Assembly_data.
  $assembly_data_collection = $connection->repository->constructObject('packersPlus:assembly_data_collection');
  $assembly_data_collection->owner = 'fedoraAdmin';
  $assembly_data_collection->label = 'Assembly Data';
  $assembly_data_collection->models = 'islandora:collectionCModel';
  $assembly_data_collection->relationships->add(
    FEDORA_RELS_EXT_URI,
    'isMemberOfCollection',
    variable_get('islandora_repository_pid', 'islandora:root')
  );
  packers_solution_pack_add_tn_datastream($assembly_data_collection);

  // Jobs.
  $jobs_collection = $connection->repository->constructObject('packersPlus:jobs');
  $jobs_collection->owner = 'fedoraAdmin';
  $jobs_collection->label = 'Jobs';
  $jobs_collection->models = 'islandora:collectionCModel';
  $jobs_collection->relationships->add(
    FEDORA_RELS_EXT_URI,
    'isMemberOfCollection',
    variable_get('islandora_repository_pid', 'islandora:root')
  );
  packers_solution_pack_add_tn_datastream($jobs_collection);

  // Miscellaneous Job Files.
  $misc_job_files_collection = $connection->repository->constructObject('packersPlus:miscellaneous_job_files');
  $misc_job_files_collection->owner = 'fedoraAdmin';
  $misc_job_files_collection->label = 'Miscellaneous Job Files';
  $misc_job_files_collection->models = 'islandora:collectionCModel';
  $misc_job_files_collection->relationships->add(
    FEDORA_RELS_EXT_URI,
    'isMemberOfCollection',
    'packersPlus:jobs'
  );
  packers_solution_pack_add_tn_datastream($misc_job_files_collection);

  // Domestic Jobs.
  $domestic_jobs_collection = $connection->repository->constructObject('packersPlus:domestic_jobs');
  $domestic_jobs_collection->owner = 'fedoraAdmin';
  $domestic_jobs_collection->label = 'Domestic Jobs';
  $domestic_jobs_collection->models = 'islandora:collectionCModel';
  $domestic_jobs_collection->relationships->add(
    FEDORA_RELS_EXT_URI,
    'isMemberOfCollection',
    'packersPlus:jobs'
  );
  packers_solution_pack_add_tn_datastream($domestic_jobs_collection);

  // International Jobs.
  $international_jobs_collection = $connection->repository->constructObject('packersPlus:international_jobs');
  $international_jobs_collection->owner = 'fedoraAdmin';
  $international_jobs_collection->label = 'International Jobs';
  $international_jobs_collection->models = 'islandora:collectionCModel';
  $international_jobs_collection->relationships->add(
    FEDORA_RELS_EXT_URI,
    'isMemberOfCollection',
    'packersPlus:jobs'
  );
  packers_solution_pack_add_tn_datastream($international_jobs_collection);

  // Miscellaneous Jobs.
  $misc_jobs_collection = $connection->repository->constructObject('packersPlus:miscellaneous_jobs');
  $misc_jobs_collection->owner = 'fedoraAdmin';
  $misc_jobs_collection->label = 'Miscellaneous Jobs';
  $misc_jobs_collection->models = 'islandora:collectionCModel';
  $misc_jobs_collection->relationships->add(
    FEDORA_RELS_EXT_URI,
    'isMemberOfCollection',
    'packersPlus:jobs'
  );
  packers_solution_pack_add_tn_datastream($misc_jobs_collection);

  // Sub Assembly Drawings.
  $assembly_drawings_collection = $connection->repository->constructObject('packersPlus:sub_assembly_drawings');
  $assembly_drawings_collection->owner = 'fedoraAdmin';
  $assembly_drawings_collection->label = 'Sub Assembly Drawings';
  $assembly_drawings_collection->models = 'islandora:collectionCModel';
  $assembly_drawings_collection->relationships->add(
    FEDORA_RELS_EXT_URI,
    'isMemberOfCollection',
    'packersPlus:jobs'
  );
  packers_solution_pack_add_tn_datastream($assembly_drawings_collection);

  // Quality Assurance Images.
  $qa_images_collection = $connection->repository->constructObject('packersPlus:quality_assurance_images');
  $qa_images_collection->owner = 'fedoraAdmin';
  $qa_images_collection->label = 'Quality Assurance Images';
  $qa_images_collection->models = 'islandora:collectionCModel';
  $qa_images_collection->relationships->add(
    FEDORA_RELS_EXT_URI,
    'isMemberOfCollection',
    'packersPlus:quality_assurance_collection'
  );
  packers_solution_pack_add_tn_datastream($qa_images_collection);

  // Shift Tests.
  $shift_collection = $connection->repository->constructObject('packersPlus:shift_tests');
  $shift_collection->owner = 'fedoraAdmin';
  $shift_collection->label = 'Shift Tests';
  $shift_collection->models = 'islandora:collectionCModel';
  $shift_collection->relationships->add(
    FEDORA_RELS_EXT_URI,
    'isMemberOfCollection',
    'packersPlus:quality_assurance_collection'
  );
  packers_solution_pack_add_tn_datastream($shift_collection);

  // Torque Tests.
  $torque_collection = $connection->repository->constructObject('packersPlus:torque_tests');
  $torque_collection->owner = 'fedoraAdmin';
  $torque_collection->label = 'Torque Tests';
  $torque_collection->models = 'islandora:collectionCModel';
  $torque_collection->relationships->add(
    FEDORA_RELS_EXT_URI,
    'isMemberOfCollection',
    'packersPlus:quality_assurance_collection'
  );
  packers_solution_pack_add_tn_datastream($torque_collection);

  // Coordinate Measurement Tests.
  $cmm_collection = $connection->repository->constructObject('packersPlus:coordinate_measurements');
  $cmm_collection->owner = 'fedoraAdmin';
  $cmm_collection->label = 'Coordinate Measurement Tests';
  $cmm_collection->models = 'islandora:collectionCModel';
  $cmm_collection->relationships->add(
    FEDORA_RELS_EXT_URI,
    'isMemberOfCollection',
    'packersPlus:quality_assurance_collection'
  );
  packers_solution_pack_add_tn_datastream($cmm_collection);

  // Faro Data.
  $faro_collection = $connection->repository->constructObject('packersPlus:faros');
  $faro_collection->owner = 'fedoraAdmin';
  $faro_collection->label = 'Faro Arm Data';
  $faro_collection->models = 'islandora:collectionCModel';
  $faro_collection->relationships->add(
    FEDORA_RELS_EXT_URI,
    'isMemberOfCollection',
    'packersPlus:quality_assurance_collection'
  );
  packers_solution_pack_add_tn_datastream($faro_collection);

  // Non-Conformance Records.
  $ncr_collection = $connection->repository->constructObject('packersPlus:NCRs');
  $ncr_collection->owner = 'fedoraAdmin';
  $ncr_collection->label = 'Non-Conformance Records';
  $ncr_collection->models = 'islandora:collectionCModel';
  $ncr_collection->relationships->add(
    FEDORA_RELS_EXT_URI,
    'isMemberOfCollection',
    variable_get('islandora_repository_pid', 'islandora:root')
  );
  packers_solution_pack_add_tn_datastream($ncr_collection);

  // QC NCRs.
  $qc_ncr_collection = $connection->repository->constructObject('packersPlus:QC_NCRs');
  $qc_ncr_collection->owner = 'fedoraAdmin';
  $qc_ncr_collection->label = 'QC NCRs';
  $qc_ncr_collection->models = 'islandora:collectionCModel';
  $qc_ncr_collection->relationships->add(
    FEDORA_RELS_EXT_URI,
    'isMemberOfCollection',
    'packersPlus:NCRs'
  );
  packers_solution_pack_add_tn_datastream($qc_ncr_collection);

  // Raw Material NCRs.
  $raw_material_ncr_collection = $connection->repository->constructObject('packersPlus:raw_material_NCRs');
  $raw_material_ncr_collection->owner = 'fedoraAdmin';
  $raw_material_ncr_collection->label = 'Raw Material NCRs';
  $raw_material_ncr_collection->models = 'islandora:collectionCModel';
  $raw_material_ncr_collection->relationships->add(
    FEDORA_RELS_EXT_URI,
    'isMemberOfCollection',
    'packersPlus:NCRs'
  );
  packers_solution_pack_add_tn_datastream($raw_material_ncr_collection);

  // Purchasing NCRs.
  $purchasing_ncr_collection = $connection->repository->constructObject('packersPlus:purchasing_NCRs');
  $purchasing_ncr_collection->owner = 'fedoraAdmin';
  $purchasing_ncr_collection->label = 'Purchasing NCRs';
  $purchasing_ncr_collection->models = 'islandora:collectionCModel';
  $purchasing_ncr_collection->relationships->add(
    FEDORA_RELS_EXT_URI,
    'isMemberOfCollection',
    'packersPlus:NCRs'
  );
  packers_solution_pack_add_tn_datastream($purchasing_ncr_collection);

  // Assembly NCRs.
  $assembly_ncr_collection = $connection->repository->constructObject('packersPlus:assembly_NCRs');
  $assembly_ncr_collection->owner = 'fedoraAdmin';
  $assembly_ncr_collection->label = 'Assembly NCRs';
  $assembly_ncr_collection->models = 'islandora:collectionCModel';
  $assembly_ncr_collection->relationships->add(
    FEDORA_RELS_EXT_URI,
    'isMemberOfCollection',
    'packersPlus:NCRs'
  );
  packers_solution_pack_add_tn_datastream($assembly_ncr_collection);

  // NCRs From database.
  $database_ncr_collection = $connection->repository->constructObject('packersPlus:database_NCRs');
  $database_ncr_collection->owner = 'fedoraAdmin';
  $database_ncr_collection->label = 'NCRs From database';
  $database_ncr_collection->models = 'islandora:collectionCModel';
  $database_ncr_collection->relationships->add(
    FEDORA_RELS_EXT_URI,
    'isMemberOfCollection',
    'packersPlus:NCRs'
  );
  packers_solution_pack_add_tn_datastream($database_ncr_collection);

  // Sales Documents.
  $sales_collection = $connection->repository->constructObject('packersPlus:sales_documents');
  $sales_collection->owner = 'fedoraAdmin';
  $sales_collection->label = 'Sales Documents';
  $sales_collection->models = 'islandora:collectionCModel';
  $sales_collection->relationships->add(
    FEDORA_RELS_EXT_URI,
    'isMemberOfCollection',
    variable_get('islandora_repository_pid', 'islandora:root')
  );
  packers_solution_pack_add_tn_datastream($sales_collection);

  // Sales Orders.
  $sales_orders_collection = $connection->repository->constructObject('packersPlus:sales_orders');
  $sales_orders_collection->owner = 'fedoraAdmin';
  $sales_orders_collection->label = 'Sales Orders';
  $sales_orders_collection->models = 'islandora:collectionCModel';
  $sales_orders_collection->relationships->add(
    FEDORA_RELS_EXT_URI,
    'isMemberOfCollection',
    'packersPlus:sales_documents'
  );
  packers_solution_pack_add_tn_datastream($sales_orders_collection);

  // International Sales Orders.
  $international_sales_orders_collection = $connection->repository->constructObject('packersPlus:international_sales_orders');
  $international_sales_orders_collection->owner = 'fedoraAdmin';
  $international_sales_orders_collection->label = 'International Sales Orders';
  $international_sales_orders_collection->models = 'islandora:collectionCModel';
  $international_sales_orders_collection->relationships->add(
    FEDORA_RELS_EXT_URI,
    'isMemberOfCollection',
    'packersPlus:sales_orders'
  );
  packers_solution_pack_add_tn_datastream($international_sales_orders_collection);

  // Sales Quotes.
  $sales_quotes_collection = $connection->repository->constructObject('packersPlus:sales_quotes');
  $sales_quotes_collection->owner = 'fedoraAdmin';
  $sales_quotes_collection->label = 'Sales Quotes';
  $sales_quotes_collection->models = 'islandora:collectionCModel';
  $sales_quotes_collection->relationships->add(
    FEDORA_RELS_EXT_URI,
    'isMemberOfCollection',
    'packersPlus:sales_documents'
  );
  packers_solution_pack_add_tn_datastream($sales_quotes_collection);

  // International Sales Quotes.
  $international_sales_quotes_collection = $connection->repository->constructObject('packersPlus:international_sales_quotes');
  $international_sales_quotes_collection->owner = 'fedoraAdmin';
  $international_sales_quotes_collection->label = 'International Sales Quotes';
  $international_sales_quotes_collection->models = 'islandora:collectionCModel';
  $international_sales_quotes_collection->relationships->add(
    FEDORA_RELS_EXT_URI,
    'isMemberOfCollection',
    'packersPlus:sales_quotes'
  );
  packers_solution_pack_add_tn_datastream($international_sales_quotes_collection);

  // Content Models.
  // Assembly CModel.
  $assembly_content_model = $connection->repository->constructObject('packersPlus:assembly');
  $assembly_content_model->owner = 'fedoraAdmin';
  $assembly_content_model->label = 'Assembly Model';
  $assembly_content_model->models = 'fedora-system:ContentModel-3.0';
  packers_solution_pack_add_islandoracm_datastream($assembly_content_model);

  // Engineering item CModel.
  $engineering_item_content_model = $connection->repository->constructObject('packersPlus:engineering_item');
  $engineering_item_content_model->owner = 'fedoraAdmin';
  $engineering_item_content_model->label = 'Engineering Item Model';
  $engineering_item_content_model->models = 'fedora-system:ContentModel-3.0';
  packers_solution_pack_add_islandoracm_datastream($engineering_item_content_model);

  // Job CModel.
  $job_content_model = $connection->repository->constructObject('packersPlus:job');
  $job_content_model->owner = 'fedoraAdmin';
  $job_content_model->label = 'Job Model';
  $job_content_model->models = 'fedora-system:ContentModel-3.0';
  packers_solution_pack_add_islandoracm_datastream($job_content_model);

  // Database NCR CModel.
  $database_ncr_model = $connection->repository->constructObject('packersPlus:database_NCR');
  $database_ncr_model->owner = 'fedoraAdmin';
  $database_ncr_model->label = 'Database NCR Model';
  $database_ncr_model->models = 'fedora-system:ContentModel-3.0';
  packers_solution_pack_add_islandoracm_datastream($database_ncr_model);

  // Part birth certificate CModel.
  $part_birth_certificate_model = $connection->repository->constructObject('packersPlus:part_birth_certificate');
  $part_birth_certificate_model->owner = 'fedoraAdmin';
  $part_birth_certificate_model->label = 'Part birth certificate Model';
  $part_birth_certificate_model->models = 'fedora-system:ContentModel-3.0';
  packers_solution_pack_add_islandoracm_datastream($part_birth_certificate_model);

  // International shipping report CModel.
  $international_shipping_report_model = $connection->repository->constructObject('packersPlus:international_shipping_report');
  $international_shipping_report_model->owner = 'fedoraAdmin';
  $international_shipping_report_model->label = 'International shipping report Model';
  $international_shipping_report_model->models = 'fedora-system:ContentModel-3.0';
  packers_solution_pack_add_islandoracm_datastream($international_shipping_report_model);

  // Domestic shipping report CModel.
  $domestic_shipping_report_model = $connection->repository->constructObject('packersPlus:domestic_shipping_report');
  $domestic_shipping_report_model->owner = 'fedoraAdmin';
  $domestic_shipping_report_model->label = 'Domestic shipping report Model';
  $domestic_shipping_report_model->models = 'fedora-system:ContentModel-3.0';
  packers_solution_pack_add_islandoracm_datastream($domestic_shipping_report_model);

  // Scrap request CModel.
  $scrap_request_model = $connection->repository->constructObject('packersPlus:scrap_request');
  $scrap_request_model->owner = 'fedoraAdmin';
  $scrap_request_model->label = 'Scrap request Model';
  $scrap_request_model->models = 'fedora-system:ContentModel-3.0';
  packers_solution_pack_add_islandoracm_datastream($scrap_request_model);

  // Tool conversion report CModel.
  $tool_conversion_model = $connection->repository->constructObject('packersPlus:tool_conversion_report');
  $tool_conversion_model->owner = 'fedoraAdmin';
  $tool_conversion_model->label = 'Tool conversion report Model';
  $tool_conversion_model->models = 'fedora-system:ContentModel-3.0';
  packers_solution_pack_add_islandoracm_datastream($tool_conversion_model);

  // Engineering change notice CModel.
  $engineering_change_model = $connection->repository->constructObject('packersPlus:engineering_change_notice');
  $engineering_change_model->owner = 'fedoraAdmin';
  $engineering_change_model->label = 'Engineering change notice Model';
  $engineering_change_model->models = 'fedora-system:ContentModel-3.0';
  packers_solution_pack_add_islandoracm_datastream($engineering_change_model);

  // MTR PDF CModel.
  $mtr_pdf_model = $connection->repository->constructObject('packersPlus:MTR_PDF');
  $mtr_pdf_model->owner = 'fedoraAdmin';
  $mtr_pdf_model->label = 'MTR PDF Model';
  $mtr_pdf_model->models = 'fedora-system:ContentModel-3.0';
  packers_solution_pack_add_islandoracm_datastream($mtr_pdf_model);

  // Assembly data CModel.
  $assembly_data_model = $connection->repository->constructObject('packersPlus:assembly_data');
  $assembly_data_model->owner = 'fedoraAdmin';
  $assembly_data_model->label = 'Assembly data Model';
  $assembly_data_model->models = 'fedora-system:ContentModel-3.0';
  packers_solution_pack_add_islandoracm_datastream($assembly_data_model);

  // Sub assembly drawing CModel.
  $sub_assebly_drawing_model = $connection->repository->constructObject('packersPlus:sub_assembly_drawing');
  $sub_assebly_drawing_model->owner = 'fedoraAdmin';
  $sub_assebly_drawing_model->label = 'Sub assembly drawing Model';
  $sub_assebly_drawing_model->models = 'fedora-system:ContentModel-3.0';
  packers_solution_pack_add_islandoracm_datastream($sub_assebly_drawing_model);

  // Quality assurance image CModel.
  $qa_image_model = $connection->repository->constructObject('packersPlus:quality_assurance_image');
  $qa_image_model->owner = 'fedoraAdmin';
  $qa_image_model->label = 'Quality assurance image Model';
  $qa_image_model->models = 'fedora-system:ContentModel-3.0';
  packers_solution_pack_add_islandoracm_datastream($qa_image_model);

  // Shift test CModel.
  $shift_test_model = $connection->repository->constructObject('packersPlus:shift_test');
  $shift_test_model->owner = 'fedoraAdmin';
  $shift_test_model->label = 'Shift test Model';
  $shift_test_model->models = 'fedora-system:ContentModel-3.0';
  packers_solution_pack_add_islandoracm_datastream($shift_test_model);

  // Shear test CModel.
  $shear_test_model = $connection->repository->constructObject('packersPlus:shear_test');
  $shear_test_model->owner = 'fedoraAdmin';
  $shear_test_model->label = 'Shear test Model';
  $shear_test_model->models = 'fedora-system:ContentModel-3.0';
  packers_solution_pack_add_islandoracm_datastream($shear_test_model);

  // Pressure test CModel.
  $pressure_test_model = $connection->repository->constructObject('packersPlus:pressure_test');
  $pressure_test_model->owner = 'fedoraAdmin';
  $pressure_test_model->label = 'Pressure test Model';
  $pressure_test_model->models = 'fedora-system:ContentModel-3.0';
  packers_solution_pack_add_islandoracm_datastream($pressure_test_model);

  // Pressure or Shear test CModel.
  $pressure__or_shear_test_model = $connection->repository->constructObject('packersPlus:Pressure_or_Shear_test');
  $pressure__or_shear_test_model->owner = 'fedoraAdmin';
  $pressure__or_shear_test_model->label = 'Pressure or Shear test Model';
  $pressure__or_shear_test_model->models = 'fedora-system:ContentModel-3.0';
  packers_solution_pack_add_islandoracm_datastream($pressure__or_shear_test_model);

  // Torque test CModel.
  $torque_test_model = $connection->repository->constructObject('packersPlus:torque_test');
  $torque_test_model->owner = 'fedoraAdmin';
  $torque_test_model->label = 'Torque test Model';
  $torque_test_model->models = 'fedora-system:ContentModel-3.0';
  packers_solution_pack_add_islandoracm_datastream($torque_test_model);

  // Coordinate measurement CModel.
  $cmm_model = $connection->repository->constructObject('packersPlus:coordinate_measurement');
  $cmm_model->owner = 'fedoraAdmin';
  $cmm_model->label = 'Coordinate measurement Model';
  $cmm_model->models = 'fedora-system:ContentModel-3.0';
  packers_solution_pack_add_islandoracm_datastream($cmm_model);

  // Faro CModel.
  $faro_model = $connection->repository->constructObject('packersPlus:faro');
  $faro_model->owner = 'fedoraAdmin';
  $faro_model->label = 'Faro Model';
  $faro_model->models = 'fedora-system:ContentModel-3.0';
  packers_solution_pack_add_islandoracm_datastream($faro_model);

  // QC NCR CModel.
  $qc_ncr_model = $connection->repository->constructObject('packersPlus:QC_NCR');
  $qc_ncr_model->owner = 'fedoraAdmin';
  $qc_ncr_model->label = 'QC NCR Model';
  $qc_ncr_model->models = 'fedora-system:ContentModel-3.0';
  packers_solution_pack_add_islandoracm_datastream($qc_ncr_model);

  // Raw material NCR CModel.
  $raw_material_ncr_model = $connection->repository->constructObject('packersPlus:raw_material_NCR');
  $raw_material_ncr_model->owner = 'fedoraAdmin';
  $raw_material_ncr_model->label = 'Raw material NCR Model';
  $raw_material_ncr_model->models = 'fedora-system:ContentModel-3.0';
  packers_solution_pack_add_islandoracm_datastream($raw_material_ncr_model);

  // Purchasing NCR CModel.
  $purchasing_ncr_model = $connection->repository->constructObject('packersPlus:purchasing_NCR');
  $purchasing_ncr_model->owner = 'fedoraAdmin';
  $purchasing_ncr_model->label = 'Purchasing NCR Model';
  $purchasing_ncr_model->models = 'fedora-system:ContentModel-3.0';
  packers_solution_pack_add_islandoracm_datastream($purchasing_ncr_model);

  // Assembly NCR CModel.
  $assembly_ncr_model = $connection->repository->constructObject('packersPlus:assembly_NCR');
  $assembly_ncr_model->owner = 'fedoraAdmin';
  $assembly_ncr_model->label = 'Assembly NCR Model';
  $assembly_ncr_model->models = 'fedora-system:ContentModel-3.0';
  packers_solution_pack_add_islandoracm_datastream($assembly_ncr_model);

  // International sales quote CModel.
  $international_sales_quote_model = $connection->repository->constructObject('packersPlus:international_sales_quote');
  $international_sales_quote_model->owner = 'fedoraAdmin';
  $international_sales_quote_model->label = 'International sales quote Model';
  $international_sales_quote_model->models = 'fedora-system:ContentModel-3.0';
  packers_solution_pack_add_islandoracm_datastream($international_sales_quote_model);

  // International sales order CModel.
  $international_sales_order_model = $connection->repository->constructObject('packersPlus:international_sales_order');
  $international_sales_order_model->owner = 'fedoraAdmin';
  $international_sales_order_model->label = 'International sales order Model';
  $international_sales_order_model->models = 'fedora-system:ContentModel-3.0';
  packers_solution_pack_add_islandoracm_datastream($international_sales_order_model);

  // MTR CModel.
  $mtr_model = $connection->repository->constructObject('packersPlus:MTR');
  $mtr_model->owner = 'fedoraAdmin';
  $mtr_model->label = 'MTR Model';
  $mtr_model->models = 'fedora-system:ContentModel-3.0';
  packers_solution_pack_add_islandoracm_datastream($mtr_model);

  // Miscellaneous job file CModel.
  $misc_job_files_model = $connection->repository->constructObject('packersPlus:miscellaneous_job_file');
  $misc_job_files_model->owner = 'fedoraAdmin';
  $misc_job_files_model->label = 'Miscellaneous job file Model';
  $misc_job_files_model->models = 'fedora-system:ContentModel-3.0';
  packers_solution_pack_add_islandoracm_datastream($misc_job_files_model);

  return array(
    'packers_solution_pack' => array(
      'title' => 'Packers',
      'objects' => array(
        $birth_certificate_collection,
        $birth_certificate_assemblies_collection,
        $birth_certificate_parts_collection,
        $engineering_items_collection,
        $quality_assurance_collection,
        $peressure_and_sheer_collection,
        $shipping_reports_collection,
        $domestic_shippping_reports_collection,
        $international_shippping_reports_collection,
        $scrap_collection,
        $tool_conversion_collection,
        $engineering_change_collection,
        $mtr_collection,
        $mtr_pdf_collection,
        $assembly_data_collection,
        $jobs_collection,
        $misc_job_files_collection,
        $domestic_jobs_collection,
        $international_jobs_collection,
        $misc_jobs_collection,
        $assembly_drawings_collection,
        $qa_images_collection,
        $shift_collection,
        $torque_collection,
        $cmm_collection,
        $faro_collection,
        $ncr_collection,
        $qc_ncr_collection,
        $raw_material_ncr_collection,
        $purchasing_ncr_collection,
        $assembly_ncr_collection,
        $database_ncr_collection,
        $sales_collection,
        $sales_orders_collection,
        $international_sales_orders_collection,
        $sales_quotes_collection,
        $international_sales_quotes_collection,
        $assembly_content_model,
        $engineering_item_content_model,
        $job_content_model,
        $database_ncr_model,
        $part_birth_certificate_model,
        $international_shipping_report_model,
        $domestic_shipping_report_model,
        $scrap_request_model,
        $tool_conversion_model,
        $engineering_change_model,
        $mtr_pdf_model,
        $assembly_data_model,
        $sub_assebly_drawing_model,
        $qa_image_model,
        $shift_test_model,
        $shear_test_model,
        $pressure_test_model,
        $pressure__or_shear_test_model,
        $torque_test_model,
        $cmm_model,
        $faro_model,
        $qc_ncr_model,
        $raw_material_ncr_model,
        $purchasing_ncr_model,
        $assembly_ncr_model,
        $international_sales_quote_model,
        $international_sales_order_model,
        $mtr_model,
        $misc_job_files_model,
      ),
    ),
  );
}

/**
* Implements hook_CMODEL_PID_islandora_solr_object_result_alter().
*/
function packers_solution_pack_packersplus_part_birth_certificate_islandora_solr_object_result_alter(&$search_results, $query_processor) {
  module_load_include('inc', 'packers_solution_pack', 'packers_solution_pack');
  $filter = packers_solution_pack_parse_contextual_filter_from_pid($search_results['solr_doc']['PID']);
  $search_results['object_url'] = 'part-view/' . $filter;
}

/**
 * Implements hook_CMODEL_PID_islandora_solr_object_result_alter().
 */
function packers_solution_pack_packersplus_assembly_islandora_solr_object_result_alter(&$search_results, $query_processor) {
  module_load_include('inc', 'packers_solution_pack', 'packers_solution_pack');
  $filter = packers_solution_pack_parse_contextual_filter_from_pid($search_results['solr_doc']['PID']);
  if (isset($search_results['solr_doc']['RELS_EXT_has_serial_number_of_literal_ms']) && !empty($search_results['solr_doc']['RELS_EXT_has_serial_number_of_literal_ms'][0])) {
    $search_results['object_url'] = 'assembly-serial-view/' . $filter;
  }
  else {
    $search_results['object_url'] = 'assembly-purchase-order-view/' . $filter;
  }
}

/**
 * Implements hook_form_FORM_ID_alter()
 */
function packers_solution_pack_form_islandora_bookmark_results_form_alter(&$form, &$form_state) {
  foreach($form['islandora_bookmark_table']['#options'] as $pid => $option) {
    if (strpos($pid, 'packersPlus:fk_') === 0) {
      $form['islandora_bookmark_table']['#options'][$pid]['#attributes'] = array('data-packers_solution_pack_disabled' => 'TRUE');
    }
  }
  drupal_add_js(drupal_get_path('module', 'packers_solution_pack') . '/js/disable_checks.js');
}

/**
 * Implements hook_block_view_alter().
 *
 * Changes powered by block for our software. Can't use delt alter with a dash
 * in the delta.
 */
function packers_solution_pack_block_view_alter(&$data, $block) {
  if ($block->delta == 'powered-by') {
    $data['content'] = preg_replace('/Powered by <a href="http:\/\/drupal.org">Drupal<\/a>/', 'Powered by Islandora and discoverygarden', $data['content']);
  }
}

/**
 * Implements hook_block_view_MODULE_DELTA_alter().
 *
 * Mangles Fedora URIs to be human readable.
 */
function packers_solution_pack_block_view_islandora_solr_basic_facets_alter(&$data, $block) {
  if (isset($data['content'])) {
    // Order is very important.
    $mapping = array(
      '/>info:fedora\//' => '>',
      '/>islandora:root/' => '>Collections',
      '/>packersPlus:Pressure_and_Shear_Data/' => '>Pressure Tests',
      '/>packersPlus:engineering_items/' => '>Engineering Items',
      '/>packersPlus:domestic_shipping_reports/' => '>Domestic Shipping',
      '/>packersPlus:international_shipping_reports/' => '>International Shipping',
      '/>packersPlus:scrap_requests/' => '>Scrap Requests',
      '/>packersPlus:tool_conversion_reports/' => '>Tool Conversions',
      '/>packersPlus:engineering_change_notices/' => '>Engineering Changes',
      '/>packersPlus:MTRs/' => '>MTR Data',
      '/>packersPlus:MTR_PDFs/' => '>MTR PDFs',
      '/>packersPlus:assembly_data_collection/' => '>Assembly Data',
      '/>packersPlus:miscellaneous_job_files/' => '>Misc Job Files',
      '/>packersPlus:domestic_jobs/' => '>Domestic Jobs',
      '/>packersPlus:sub_assembly_drawings/' => '>Sub-Assemblies',
      '/>packersPlus:quality_assurance_images/' => '>QA Images',
      '/>packersPlus:shift_tests/' => '>Shift Tests',
      '/>packersPlus:torque_tests/' => '>Torque Tests',
      '/>packersPlus:coordinate_measurements/' => '>Coordinate Data',
      '/>packersPlus:QC_NCRs/' => '>QC NCRs',
      '/>packersPlus:raw_material_NCRs/' => '>RM NCRs',
      '/>packersPlus:international_sales_orders/' => '>International Orders',
      '/>packersPlus:international_sales_quotes/' => '>International Quotes',
      // Ones not in Mark's list.
      '/>packersPlus:sales_quotes/' => '>Sales Quotes',
      '/>packersPlus:part_birth_certificates/' => '>Part Birth Certificates',
      '/>packersPlus:purchasing_NCRs/' => '>Purchasing NCRs',
      '/>packersPlus:jobs/' => '>Jobs',
      '/>packersPlus:assembly_NCRs/' => '>Assembly NCRs',
      '/>packersPlus:birth_certificates/' => '>Birth Certificates',
      '/>packersPlus:assemblies/' => '>Assemblies',
      '/>packersPlus:international_jobs/' => '>International Jobs',
      '/>packersPlus:quality_assurance_collection/' => '>Quality Assurance Collection',
      '/>packersPlus:shipping_reports/' => '>Shipping Reports',
      '/>packersPlus:miscellaneous_jobs/' => '>Miscellaneous Jobs',
      '/>packersPlus:database_NCRs/' => '>NCRs From database',
      '/>packersPlus:faros/' => '>Faro Arm Data',
      '/>packersPlus:NCRs/' => '>Non-Conformance Records',
      '/>packersPlus:sales_documents/' => '>Sales Documents',
      '/>packersPlus:sales_orders/' => '>Sales Orders',
    );
    foreach ($mapping as $match => $replace) {
      $data['content'] = preg_replace($match, $replace, $data['content']);
    }
  }
}

function packers_solution_main_search_block_form($form, &$form_state){
  $search_form['keywords'] = array(
    '#type' => 'textfield',
    '#title' => t('Containing these keywords'),
    '#default_value' => t('Enter one or more keywords'),
    '#size' => 60,
    '#maxlength' => 128,
    '#required' => TRUE,
  );

  $search_form['submit'] = array('#type' => 'submit', '#value' => t('Search'));

  $master_collections['packersPlus:birth_certificates'] = array(
    '#type' => 'checkbox',
    '#title' => t('Birth Certificates')
  );
  $master_collections['packersPlus:assemblies'] = array(
    '#type' => 'checkbox',
    '#title' => t('Assembly Data')
  );
  $master_collections['packersPlus:MTRs'] = array(
    '#type' => 'checkbox',
    '#title' => t('MTRs')
  );
#  $master_collections['search_all_collections'] = array(
#    '#type' => 'checkbox',
#    '#title' => t('Search All Collections')
#  );
  $master_collections['packersPlus:scrap_requests'] = array(
    '#type' => 'checkbox',
    '#title' => t('Scrap Requests')
  );
  $master_collections['packersPlus:tool_conversion_reports'] = array(
    '#type' => 'checkbox',
    '#title' => t('Tool Conversion Reports')
  );
#  $master_collections['packersPlus:part_birth_certificates'] = array(
#    '#type' => 'checkbox',
#    '#title' => t('Part Birth Certificates')
#  );
#  $master_collections['packersPlus:engineering_items'] = array(
#    '#type' => 'checkbox',
#    '#title' => t('Engineering Items')
#  );
    $qa_collection['packersPlus:Pressure_and_Shear_Data'] = array(
    '#type' => 'checkbox',
    '#title' => t('Pressure and Shear Data')
  );
  $horizon_form['packersPlus:quality_assurance_collection'] = array(
    '#type' => 'checkbox',
    '#title' => t('Quality Assurance Collection')
  );
  $horizon_form['packersPlus:shipping_reports'] = array(
    '#type' => 'checkbox',
    '#title' => t('Shipping Reports')
  );
  $shipping_requests['packersPlus:domestic_shipping_reports'] = array(
    '#type' => 'checkbox',
    '#title' => t('Domestic Shipping Reports')
  );
  $shipping_requests['packersPlus:international_shipping_reports'] = array(
    '#type' => 'checkbox',
    '#title' => t('International Shipping Reports')
  );
  $horizon_form['packersPlus:engineering_change_notices'] = array(
    '#type' => 'checkbox',
    '#title' => t('Engineering Change Notices')
  );
#  $horizon_form['packersPlus:MTR_PDFs'] = array(
#    '#type' => 'checkbox',
#    '#title' => t('MTR PDFs')
#  );
#  $horizon_form['packersPlus:assembly_data_collection'] = array(
#    '#type' => 'checkbox',
#    '#title' => t('Assembly Data')
#  );
#  $horizon_form['packersPlus:jobs'] = array(
#    '#type' => 'checkbox',
#    '#title' => t('Jobs')
#  );
  $jobs['packersPlus:sub_assembly_drawings'] = array(
    '#type' => 'checkbox',
    '#title' => t('Sub Assembly Drawings')
  );

  $jobs['packersPlus:miscellaneous_job_files'] = array(
    '#type' => 'checkbox',
    '#title' => t('Miscellaneous Job Files')
  );
    $jobs['packersPlus:domestic_jobs'] = array(
    '#type' => 'checkbox',
    '#title' => t('Domestic Jobs')
  );
  $jobs['packersPlus:international_jobs'] = array(
    '#type' => 'checkbox',
    '#title' => t('International Jobs')
  );
  $jobs['packersPlus:miscellaneous_jobs'] = array(
    '#type' => 'checkbox',
    '#title' => t('Miscellaneous Jobs')
  );
  $qa_collection['packersPlus:quality_assurance_images'] = array(
    '#type' => 'checkbox',
    '#title' => t('Quality Assurance Images')
  );
  $qa_collection['packersPlus:shift_tests'] = array(
    '#type' => 'checkbox',
    '#title' => t('Shift Tests')
  );
  $qa_collection['packersPlus:torque_tests'] = array(
    '#type' => 'checkbox',
    '#title' => t('Torque Tests')
  );
  $qa_collection['packersPlus:coordinate_measurements'] = array(
    '#type' => 'checkbox',
    '#title' => t('Coordinate Measurement Tests')
  );
  $qa_collection['packersPlus:faros'] = array(
    '#type' => 'checkbox',
    '#title' => t('Faro Arm Data')
  );
  $ncr['packersPlus:NCRs'] = array(
    '#type' => 'checkbox',
    '#title' => t('Non-Conformance Records')
  );
  $ncr['packersPlus:QC_NCRs'] = array(
    '#type' => 'checkbox',
    '#title' => t('QC NCRs')
  );
  $ncr['packersPlus:raw_material_NCRs'] = array(
    '#type' => 'checkbox',
    '#title' => t('Raw Material NCRs')
  );
  $ncr['packersPlus:purchasing_NCRs'] = array(
    '#type' => 'checkbox',
    '#title' => t('Purchasing NCRs')
  );
  $ncr['packersPlus:assembly_NCRs'] = array(
    '#type' => 'checkbox',
    '#title' => t('Assembly NCRs')
  );
  $ncr['packersPlus:database_NCRs'] = array(
    '#type' => 'checkbox',
    '#title' => t('NCRs From database')
  );
#  $sales_documents['packersPlus:sales_documents'] = array(
#    '#type' => 'checkbox',
#    '#title' => t('Sales Documents')
#  );
 # $sales_documents['packersPlus:sales_orders'] = array(
 #   '#type' => 'checkbox',
 #   '#title' => t('Sales Orders')
#  );
  $sales_documents['packersPlus:international_sales_orders'] = array(
    '#type' => 'checkbox',
    '#title' => t('International Sales Orders')
  );
#  $sales_documents['packersPlus:sales_quotes'] = array(
#    '#type' => 'checkbox',
#    '#title' => t('Sales Quotes')
#  );
  $sales_documents['packersPlus:international_sales_quotes'] = array(
    '#type' => 'checkbox',
    '#title' => t('International Sales Quotes')
);
  $form['search_line'] = array(
    '#type' => 'item',
 //   '#title' => t('Limit my search to this Collection'),
    '#prefix' => '<div class="form-item_keywords">',
    '#suffix' => '</div>',
    '#tree' => TRUE,
    'collection' => $search_form,
  );

  $form['collection'] = array(
    '#type' => 'item',
    '#title' => t('Limit my search to this Collection'),
    '#prefix' => '<div class="master_collections">',
    '#suffix' => '</div>',
    '#tree' => TRUE,
    'collection' => $master_collections,
  );

#  $form['horizon_form'] = array( #    '#type' => 'item',
#    '#title' => t('Collections'),
#    '#prefix' => '<div class="column_group">',
#    '#suffix' => '</div>',
#    '#tree' => TRUE,
#    'collection' => $horizon_form,
#  );


  $form['jobs'] = array(
    '#type' => 'item',
    '#title' => t('Jobs'),
    '#prefix' => '<div class="column_group">',
    '#suffix' => '</div>',
    '#tree' => TRUE,
    'collection' => $jobs,
  );
  $form['ncr'] = array(
    '#type' => 'item',
    '#title' => t('NCRs'),
    '#prefix' => '<div class="column_group">',
    '#suffix' => '</div>',
    '#tree' => TRUE,
    'collection' => $ncr,
  );
  $form['qa_collection'] = array(
    '#type' => 'item',
    '#title' => t('Quality Assurance Collection'),
    '#prefix' => '<div class="column_group">',
    '#suffix' => '</div>',
    '#tree' => TRUE,
    'collection' => $qa_collection,
  );

  $form['shipping_requests'] = array(
    '#type' => 'item',
    '#title' => t('Shipping Requests'),
    '#prefix' => '<div class="column_group">',
    '#suffix' => '</div>',
    '#tree' => TRUE,
    'collection' => $shipping_requests,
   );
  $form['sales_documents'] = array(
    '#type' => 'item',
    '#title' => t('Sales Documents'),
    '#prefix' => '<div class="column_group">',
    '#suffix' => '</div>',
    '#tree' => TRUE,
    'collection' => $sales_documents,
  );
  if(!isset($form_state['storage']['additional_search_boxes'])){
    $form_state['storage']['additional_search_boxes'] = 0 ;
  }

  $number = ++$form_state['storage']['additional_search_boxes'] ;
  for($i = 0; $i < $number; $i++ ){
      $additional_items[$i]['title'] = array(
        '#type' => 'textfield',
        '#title' => t('Keyword(s)'),
        '#default_value' => t('Enter one or more serial number'),
        '#size' => 60,
        '#maxlength' => 128,
        '#required' => FALSE,
      );

      $additional_items[$i]['selected'] = array(
        '#type' => 'select',
        '#title' => t('Selected'),
        '#options' => array(
            'collection' => t('Collection'),
            'serial_number' => t('Serial Number'),
            'purchase_number' => t('Purchase Order #'),
            'production_number' => t('Production Order #'),
            'part_number' => t('Part Number'),
            'heat_number' => t('Heat Number'),
            'file_name' => t('File Name'),
            'path_file_name' => t('Path and File Name'),
            'type_of_document' => t('Type of Document'),
            'database_of_origin' => t('Database of Origin'),
            'job_pid' => t('Job Pid'),
            'mtr_pid' => t('MTR Pid'),
            'assembly_pid' => t('Assembly Pid'),
            'job_identifier' => t('Job Identifier'),
            'object_pid' => t('Object Pid'),
            ),
    //        '#default_value' => $category['selected'],
      );
    }

  $additional_items['add_item'] = array(
    '#type' => 'button',
    '#value' => t('Add Item'),
    '#ajax' => array(
       'wrapper' => 'additional_items',
       'callback' => 'packers_solution_pack_add_more_searches',
    ),
  );

  //$form['additional_items'] = array(
      //'#type' => 'item',
      //'#title' => t('Build a Search'),
      //'#prefix' => '<div id="additional_items">',
      //'#suffix' => '</div>',
      //'#tree' => TRUE,
      //'collection' => $additional_items,
  //);

  return $form ;
}


function packers_solution_pack_add_more_searches($form, &$form_state){
    return $form['additional_items'];
}

function packers_solution_main_search_block_form_submit($form, &$form_state){
  //get all the checked boxes
  $collections = array_filter($form_state['values']['collection']['collection'], 'get_checked');
  // Get rid of birth certs, we'll check for those later
  if (isset($collections['packersPlus:birth_certificates'])) {
    unset($collections['packersPlus:birth_certificates']);
  }
  $jobs = array_filter($form_state['values']['jobs']['collection'], 'get_checked');
  $ncr = array_filter($form_state['values']['ncr']['collection'], 'get_checked');
  $qa_collection = array_filter($form_state['values']['qa_collection']['collection'], 'get_checked');
  $sales_documents = array_filter($form_state['values']['sales_documents']['collection'], 'get_checked');
  $shipping_requests = array_filter($form_state['values']['shipping_requests']['collection'], 'get_checked');
  //$misc_requests = array_filter($form_state['values']['horizon_form']['collection'], 'get_checked');

  //$search_all_collections = $form_state['values']['collection']['collection']['search_all_collections'] ;
  //and then merge
  $collections_to_search = array_keys(array_merge($collections, $jobs, $ncr, $qa_collection, $sales_documents, $shipping_requests));

  foreach($collections_to_search as &$val){
    $val = 'info:fedora/' . $val ;
  }
  $collection_search_string = implode('" || "', $collections_to_search);
  $query_string = '';
  if (strlen($form_state['values']['search_line']['collection']['keywords']) > 0) {
    $least_one = $form_state['values']['search_line']['collection']['keywords'];
    $least_array = explode(' ', $least_one);
    $count = 0;
    //if (strlen($query_string)) {
      //$query_string .= ' && ';
    //}
    foreach ($least_array as $word) {
      if ($count == 0) {
        $query_string .= '(' . $word;
      }
      else {
        $query_string .= ' || ' . $word;
      }
      $count++;
    }
    $query_string .= ')';
  }

  $query_string = str_replace('/', '~slsh~', $query_string);
  $query = array(
    'type' => 'edismax',
//    'hidden_filter' => array('+RELS_EXT_hasModel_uri_ms:("'. $collection_search_string .'")'),
  );
  if(count($collections_to_search) > 0 ){
    $query['hidden_filter'] = array('RELS_EXT_isMemberOfCollection_uri_ms:("'. $collection_search_string .'")');

  }

  // And here's the birth certs I promised.
  if ($form_state['values']['collection']['collection']['packersPlus:birth_certificates']) {
      if (!isset($query['hidden_filter'])) {
        $query['hidden_filter'] = array('');
      }
      else {
        $query['hidden_filter'][0] .= ' || ';
      }

      $content_models = array(
          'info:fedora/packersPlus:part_birth_certificate',
          'info:fedora/packersPlus:assembly',
      );
      $content_models_string = implode('" || "', $content_models);
      $query['hidden_filter'][0] .= 'RELS_EXT_hasModel_uri_ms:("' . $content_models_string . '")';
  }

  drupal_goto('islandora/search/' . $query_string, array(
    'query' => $query,
  ));

}

function get_checked($var){
    return $var;
}
